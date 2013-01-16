#pragma region Description
/********************************************************************************\
[+][::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::][+]
Sockets Library by lowliet | 15.01.2013 ][:::::::::::::::::::::::::::::::::::::::]

Synopsis ][::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::]
	Aim of this project is to provide foundation for using Sockets library.

Known Issues ][::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::]
	1. none

To Do ][:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::]
	1. none

License ][:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::]
	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing,
	software distributed under the License is distributed on an
	"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
	KIND, either express or implied.  See the License for the
	specific language governing permissions and limitations
	under the License.

[+][::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::][+]
\********************************************************************************/
#pragma endregion
#pragma once

#include <cstdio>
#include <string>

#if defined(_WIN32) || defined(_WIN64)
	#include <WinSock2.h>
	#include <Mswsock.h>
#elif __linux
	#include <string.h>
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>

	#define	closesocket(sock)	close(sock)
#else
	#error "SYSTEM NOT SUPPORTED"
#endif

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")

typedef struct
{
	void			Reset()
					{
						sock = -1;
						port = 0;
						address = "";
					}

	int				sock;
	unsigned short	port;
	std::string		address;
} connectionData;

class Host
{
public:
					Host();

	bool			Connect(std::string address, unsigned short port);
	bool			Disconnect();

private:
	bool			InitializeSockets();

	connectionData	hostData;
};