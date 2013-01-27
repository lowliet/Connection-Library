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

/**
 *	Class providing simple functions for Sockets library
 */
class Host
{
public:
	/**
	 *	Structure used in Receive function
	 */
	typedef struct ReceiveStruct
	{
		/**
		 *	A constructor
		 */
								ReceiveStruct() : Int(0), Char(0), String(""), Double(0.0), LongInt(0), UnsignedLongInt(0), isInitialized(false) {}
		int						Int;
		char					Char;
		std::string				String;
		double					Double;
		long int				LongInt;
		unsigned long int		UnsignedLongInt;
		bool					isInitialized;
	} receiveStruct;

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
	 *	Receives single value
	 *	@return received structure
	 *	@see Send()
	 */
	Host::receiveStruct			Receive() const;
	/**
	 *	Receives specified number of bytes
	 *	@param length number of bytes to receive
	 *	@param exactLength when true, function will not return until exact number of bytes is read
	 *	@return received data
	 *	@see Send()
	 */
	std::vector<unsigned char>	Receive(int length, bool exactLength = false) const;
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
	 *	Listens on specified port
	 *	@param port port number
	 *	@return true if successful, false otherwise
	 *	@see Accept()
	 */
	bool						Listen(unsigned short port);
	/**
	 *	Accepts client connections
	 *	@return pointer to connected host object (needs to be deleted afterwards)
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