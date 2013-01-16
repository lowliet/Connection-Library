#include "Host.h"

Host::Host()
{
	this->InitializeSockets();
}

bool Host::InitializeSockets()
{
	hostData.Reset();
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

	if (this->hostData.sock != -1) return false;

	this->hostData.sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
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

	if (connect(this->hostData.sock, (struct sockaddr*)&sockInfo, sizeof(struct sockaddr)) == -1)
	{
		closesocket(this->hostData.sock);
		return false;
	}	

	hostData.port = port;
	hostData.address = address;

	return true;
}

bool Host::Disconnect()
{
	if (this->hostData.sock == -1) return false;
	closesocket(this->hostData.sock);
	hostData.Reset();
	return true;
}