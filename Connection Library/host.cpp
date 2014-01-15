#include "host.h"

Host::Host()
{
	this->Reset();
	this->InitializeSockets();
}

Host::~Host()
{
	this->Disconnect();
}

bool Host::InitializeSockets()
{
#if (defined(_WIN32) || defined(_WIN64))
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData))
		return false;
#endif
	return true;
}

bool Host::Connect(std::string address, unsigned short port)
{
	struct sockaddr_in sockInfo;

	if (this->sock != -1) return false;

	this->sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	sockInfo.sin_family = AF_INET;
	sockInfo.sin_addr.s_addr = inet_addr(address.c_str());
	sockInfo.sin_port = htons(port);
	memset(sockInfo.sin_zero, 0, sizeof(sockInfo.sin_zero));

	#pragma region Address resolving
	if (sockInfo.sin_addr.s_addr == INADDR_NONE)
	{
		struct hostent *clientHost = gethostbyname(address.c_str());
		if (clientHost != 0) memcpy(&sockInfo.sin_addr, clientHost->h_addr_list[0], clientHost->h_length);
	}
	#pragma endregion

	if (connect(this->sock, (struct sockaddr*)&sockInfo, sizeof(struct sockaddr)) == -1)
	{
		closesocket(this->sock);
		Reset();
		return false;
	}	

	this->port = port;
	this->address = inet_ntoa(sockInfo.sin_addr);

	return true;
}

bool Host::Disconnect()
{
	if (this->sock == -1) return false;
	closesocket(this->sock);
	Reset();
	return true;
}

std::vector<unsigned char> Host::Receive(int length, bool exactLength) const
{
	std::vector<unsigned char> data(length, 0);
	int receivedBytes = -1;
	data.resize((receivedBytes = this->Receive(&data.front(), length, exactLength)) > 0 ? receivedBytes : 0);
	return data;
}

int Host::Receive(void *buffer, int length, bool exactLength) const
{
	if (this->sock == -1) return -1;
	int received = 0;
	do
	{
		int rec = recv(this->sock, (char*)buffer + received, length - received, 0);
		if (rec < 0) return -1;
		received += rec;
	} while ((received < length) && exactLength);
	return received;
}

int Host::Send(const void *buffer, int length) const
{
	if (this->sock == -1) return -1;
	if (length == 0) length = strlen((const char*)buffer);
	int sended = 0;
	do
	{
		int ret = send(this->sock, (const char*)buffer, length, 0);
		if (ret == 0) continue;
		else if (ret < 0)
		{
		#if (defined(_WIN32) || defined(_WIN64))
			if (WSAGetLastError() == WSAEWOULDBLOCK) continue;
		#elif __linux
			if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) continue;
		#endif
			return -1;
		}
		sended += ret;
	} while (sended < length);
	return sended;
}

bool Host::Listen(unsigned short port)
{
	if (port == 0) return false;

	sockaddr_in sockInfo;

	sockInfo.sin_family = AF_INET;
	sockInfo.sin_addr.s_addr = htonl(INADDR_ANY);
	sockInfo.sin_port = htons(port);

	if ((this->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) return false;
	if (bind(this->sock, (sockaddr*)&sockInfo, sizeof(sockaddr)) == -1) return false;
	if (listen(this->sock, SOMAXCONN) == -1) return false;
	
	this->port = port;

	return true;
}

Host* Host::Accept()
{
	sockaddr_in sockInfo;
	int remoteSocket;

#if (defined(_WIN32) || defined(_WIN64))
	int size;
#elif __linux
	socklen_t size;
#endif

	size = sizeof(sockaddr);
	if ((remoteSocket = accept(this->sock, (sockaddr*)&sockInfo, &size)) == -1) 
		return 0;

	Host *host = new Host();
	host->sock = remoteSocket;
	host->port = this->port;
	host->address = inet_ntoa(sockInfo.sin_addr);
	return host;
}

std::string Host::GetIPAddress() const
{
	return address;
}

int Host::Send(std::vector<unsigned char> data) const
{
	return this->Send((const void*)&data.front(), data.size());
}

void Host::Reset()
{
	this->sock = -1;
	this->port = 0;
	this->address.resize(16, '\0');
}

unsigned short Host::GetPort() const
{
	return this->port;
}

bool Host::SendFile(std::string localFileName) const
{
	if (this->sock == -1) return false;
	FILE *file = fopen(localFileName.c_str(), "rb");
	if (file == NULL) return false;
	
	#pragma region File size check
	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	fseek(file, 0, SEEK_SET);

	if (size <= 0)
	{
		fclose(file);
		return false;
	}
	#pragma endregion

	#pragma region Sending file size and name
	std::string fileSize(256, '\0');
	sprintf((char*)fileSize.c_str(), "F|%lu|%s", size, localFileName.c_str());
	if (this->Send(fileSize.c_str()) != strlen(fileSize.c_str()))
	{
		fclose(file);
		return false;
	}
	#pragma endregion

	#pragma region Waiting for proceed command
	if (this->Receive().size() < 3) 
	{
		fclose(file);
		return false;
	}
	#pragma endregion

	#pragma region Sending a file
	long bytesSended = 0;
	std::string fileChunk(1024, '\0');
	while (!feof(file))
		bytesSended += this->Send(fileChunk.c_str(), fread((char*)fileChunk.c_str(), 1, fileChunk.size() - 1, file));
	fclose(file);
	#pragma endregion

	return bytesSended == size;
}

bool Host::ReceiveFile(std::string &localFileName) const
{
	if (this->sock == -1) return false;
	std::string fileName(512, '\0');
	long fileSize, bytesReceived = 0;
	char sign;

	#pragma region Receiving file size and name
	sscanf(this->Receive().c_str(), "%c|%lu|%s", &sign, &fileSize, (char*)fileName.c_str());
	#pragma endregion

	FILE *file = fopen(localFileName.length() == 0 ? fileName.c_str() : localFileName.c_str(), "wb");
	if (file == NULL) return false;

	if (localFileName.length() == 0)
		localFileName = fileName;

	this->Send("PROCEED");

	#pragma region Receiving a file
	do
	{
		std::vector<unsigned char> fileChunk = this->Receive(fileSize - bytesReceived, true);
		if (fileChunk.size() > 0)
		{
			bytesReceived += fileChunk.size();
			fwrite((const unsigned char*)fileChunk.data(), 1, fileChunk.size(), file);
		}
		else break;
	} while (bytesReceived < fileSize);
	#pragma endregion

	fclose(file);

	return bytesReceived == fileSize;
}

std::string Host::Receive(int receiveBytesCount) const
{
	std::string ret(receiveBytesCount == 0 ? 1024 : receiveBytesCount, '\0');
	this->Receive((char*)ret.c_str(), receiveBytesCount == 0 ? ret.size() : receiveBytesCount, receiveBytesCount > 0);
	return ret;
}

int Host::Send(std::string data) const
{
	return this->Send(data.c_str(), strlen(data.c_str()));
}