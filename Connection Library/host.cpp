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

std::vector<char> Host::Receive(int length) const
{
	std::vector<char> data(length, 0);
	int receivedBytes = -1;
	data.resize((receivedBytes = this->Receive(&data.front(), length)) > 0 ? receivedBytes : 0);
	return data;
}

int Host::Receive(void *buffer, int length) const
{
	if (this->sock == -1) return -1;
	return recv(this->sock, (char*)buffer, length, 0);
}

int Host::Send(const void *buffer, int length) const
{
	if (this->sock == -1) return -1;
	if (length == 0) length = strlen((const char*)buffer);
	return send(this->sock, (const char*)buffer, length, 0);
}

bool Host::Listen(unsigned short port)
{
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

int Host::Send(std::vector<char> data) const
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