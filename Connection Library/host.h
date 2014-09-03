#pragma region Description
/********************************************************************************\
[+][::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::][+]
Sockets Library by lowliet | 15.01.2013 ][:::::::::::::::::::::::::::::::::::::::]

Synopsis ][::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::]
	Aim of this project is to provide foundation for using Sockets library.

Known Issues ][::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::]
	1. Getting file name works only on windows

To Do ][:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::]
	1. UDP handling

	2. When listening, sending and receiving should be restricted

	3. Receiving a file

	4. In Receive function, recv may return 0

	5. Using CRC or MD5 while sending files

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
	#include <errno.h>
	#include <netdb.h>

	#define	closesocket(sock)	close(sock)
#else
	#error "SYSTEM NOT SUPPORTED"
#endif

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma warning (disable : 4996)

/**
 *	Class providing basic functions for Sockets library
 */
class Host
{
public:
	/**
	 *	Constructor
	 */
								Host();
	/**
	 *	Destructor
	 */
								~Host();
	/**
	 *	Connects to host on specified port
	 *	@param address host domain name or IP address
	 *	@param port host port number
	 *	@return true if successful, false otherwise
	 *	@see Disconnect()
	 */
	bool						Connect(std::string address, unsigned short port);
	/**
	 *	Closes connection with host
	 *	@return true if successful, false otherwise
	 *	@see Connect()
	 */
	bool						Disconnect();
	/**
	 *	Receives single text line
	 *	@param length number of bytes to receive, if number is not specified, data being received is expected to be no longer than 2048 bytes
	 *	@param exactLength when true, function will not return until exact number of bytes is read
	 *	@return received text
	 *	@see Send()
	 */
	std::string					Receive(int length = 0, bool exactLength = false) const;
	/**
	 *	Receives specified number of bytes and saves it in buffer
	 *	@param buffer buffer for data
	 *	@param length number of bytes to receive
	 *	@param exactLength when true, function will not return until exact number of bytes is read
	 *	@return number of bytes received, negative value on error
	 *	@see Send()
	 */
	int							Receive(void *buffer, int length, bool exactLength = false) const;
	/**
	 *	Sends data to host
	 *	@param buffer buffer containing data
	 *	@param length number of bytes to send (if zero, buffer will be treated as a null-terminated string)
	 *	@return number of bytes sent, negative value on error
	 *	@see Receive()
	 */
	int							Send(const void *buffer, int length = 0) const;
	/**
	 *	Sends data to host
	 *	@param data vector containing data
	 *	@return number of bytes sent, negative value on error
	 *	@see Receive()
	 */
	int							Send(std::vector<unsigned char> data) const;
	/**
	 *	Sends null terminated string to host
	 *	@param data string containing null terminated text
	 *	@return true if whole string was sent, false otherwise
	 *	@see Receive()
	 */
	int							Send(std::string data) const;
	/**
	 *	Listens on specified port
	 *	@param port port number
	 *	@return true if successful, false otherwise
	 *	@see Accept()
	 */
	bool						Listen(unsigned short port);
	/**
	 *	Accepts client connections
	 *	@return pointer to connected host object (needs to be deleted afterwards) or NULL pointer
	 *	@see Listen()
	 */
	Host*						Accept();
	/**
	 *	Returns IP address
	 *	@return IP address as a string
	 */
	std::string					GetIPAddress() const;
	/**
	 *	Returns port number
	 *	@return port number
	 */
	unsigned short				GetPort() const;
	/**
	 *	Sends local file to host
	 *	@param localFileName absolute or relative file path, along with file name
	 *	@return true if successful, false otherwise
	 *	@see ReceiveFile()
	 */
	bool						SendFile(std::string localFileName) const;
	/**
	 *	Receives file
	 *	@param localFileName desired file name. If empty, it will return original file name
	 *	@return true if receiving was successful, false otherwise
	 */
	bool						ReceiveFile(std::string &localFileName) const;

private:
	/**
	 *	Initializes Sockets library
	 *	@return true if successful, false otherwise
	 */
	bool						InitializeSockets();
	/**
	 *	Sets connection parameters to default values
	 */
	void						Reset();
	/**
	 *	Socket handle
	 */
	int							sock;
	/**
	 *	Port number
	 */
	unsigned short				port;
	/**
	 *	Host IP address
	 */
	std::string					address;
};