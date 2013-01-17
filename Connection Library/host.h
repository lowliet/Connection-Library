#pragma region Description
/********************************************************************************\
[+][::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::][+]
Sockets Library by lowliet | 15.01.2013 ][:::::::::::::::::::::::::::::::::::::::]

Synopsis ][::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::]
	Aim of this project is to provide foundation for using Sockets library.

Known Issues ][::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::]
	1. none

To Do ][:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::]
	1. In Accept function convert ip from int to string

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
#include <vector>

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

/**
 * Structure holding connection parameters
 */
typedef struct
{
	/**
	 *	Sets connection parameters to default values
	 */
	void				Reset()
						{
							sock = -1;
							port = 0;
							address.resize(16, '\0');
						}
	/**
	 *	Socket handle
	 */
	int					sock;
	/**
	 *	Port number
	 */
	unsigned short		port;
	/**
	 *	Host ip address
	 */
	std::string			address;
} connectionData;

/**
 * Class providing simple functions for Sockets library
 */
class Host
{
public:
	/**
	 *	A constructor
	 */
						Host();
	/**
	 *	A destructor
	 */
						~Host();
	/**
	 *	Connects to host on specified port
	 *	@param address host domain name or ip address
	 *	@param port host port number
	 *	@return true if successful, false otherwise
	 *	@see Disconnect()
	 */
	bool				Connect(std::string address, unsigned short port);
	/**
	 *	Closes connection with host
	 *	@return true if successful, false otherwise
	 *	@see Connect()
	 */
	bool				Disconnect();
	/**
	 *	Receives specified number of bytes
	 *	@param length number of bytes to receive
	 *	@return received data
	 *	@see Send()
	 */
	std::vector<char>	Receive(int length) const;
	/**
	 *	Receives specified number of bytes and saves it in buffer
	 *	@param buffer buffer for data
	 *	@param length number of bytes to receive
	 *	@return number of bytes received, negative value on error
	 *	@see Send()
	 */
	int					Receive(void *buffer, int length) const;
	/**
	 *	Sends data to host
	 *	@param buffer buffer containing data
	 *	@param length number of bytes to send (if zero, buffer will be treated as a null-terminated string)
	 *	@return number of bytes sended, negative value on error
	 *	@see Receive()
	 */
	int					Send(const void *buffer, int length = 0) const;
	/**
	 *	Sends data to host
	 *	@param data vector containing data
	 *	@return number of bytes sended, negative value on error
	 *	@see Receive()
	 */
	int					Send(std::vector<char> data) const;
	/**
	 *	Listens on specified port
	 *	@param port port number
	 *	@return true if successful, false otherwise
	 *	@see Accept()
	 */
	bool				Listen(unsigned short port);
	/**
	 *	Accepts client connections
	 *	@return pointer to connected host object (needs to be deleted afterwards)
	 *	@see Listen()
	 */
	Host*				Accept();
	/**
	 *	Returns IP address
	 *	@return IP address as a string
	 */
	std::string			GetIPAddress() const;

private:
	/**
	 *	Initializes Sockets library
	 *	@return true if successful, false otherwise
	 */
	bool				InitializeSockets();
	/**
	 *	Structure holding basic connection parameters
	 */
	connectionData		hostData;
};