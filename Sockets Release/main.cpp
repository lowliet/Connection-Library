#if (defined(_WIN32) || defined(_WIN64)) && defined(_DEBUG)
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif

#include "../Connection Library/host.h"
#pragma comment(lib, "Connection Library")

#if (defined(_WIN32) || defined(_WIN64)) && defined(_DEBUG)
	#ifdef _DEBUG
		#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
		#define new DEBUG_NEW
	#endif
#endif

void ClientThread(LPVOID lpParam)
{
	Sleep(1000);

	Host client;

	if (client.Connect("localhost", 0xff))
	{
		printf("Client | connected to %s on %i\n", client.GetIPAddress().c_str(), client.GetPort());
		printf("Client | sent: %i bytes\n", client.Send("Data from client"));
		printf("Client | read: %s\n", client.Receive().c_str());
		printf("Client | sent: %i bytes\n", client.Send("Last message from client"));
		printf("Client | read: %s\n", client.Receive().c_str());
		printf("Client | file: %s\n", client.SendFile("test.txt") ? "good" : "bad");

		if (client.Disconnect()) puts("Client | connection closed");
		else puts("Client | cannot close connection");
	}
	else puts("Client | cannot connect");
}

void ServerThread(LPVOID lpParam)
{
	Host server;
	std::string file = "text.out";

	if (server.Listen(0xff)) 
	{
		puts("Server | listening...");

		Host *connectedHost = server.Accept();

		printf("Server | connected to %s on %i\n", connectedHost->GetIPAddress().c_str(), connectedHost->GetPort());
		printf("Server | read: %s\n", connectedHost->Receive().c_str());
		printf("Server | sent: %i bytes\n", connectedHost->Send("Data from server"));
		printf("Server | read: %s\n", connectedHost->Receive().c_str());
		printf("Server | sent: %i bytes\n", connectedHost->Send("Last message from server"));
		printf("Server | file: %s | name: ", connectedHost->ReceiveFile(file) ? "good" : "bad");
		puts(file.c_str());

		if (connectedHost->Disconnect()) puts("Server | connection closed");
		else puts("Server | cannot close connection");

		delete connectedHost;

		if (server.Disconnect()) puts("Server | socket closed");
		else puts("Server | cannot close socket");
	}
	else puts("Server | cannot listen");
}

int main()
{
	#if (defined(_WIN32) || defined(_WIN64)) && defined(_DEBUG)
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	#endif

	DWORD threadCode;

	HANDLE handleServer = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ServerThread, 0, 0, &threadCode);
	HANDLE handleClient = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ClientThread, 0, 0, &threadCode);

	if (WaitForSingleObject(handleClient, 10000) != WAIT_OBJECT_0)
		TerminateThread(handleClient, 0);
	CloseHandle(handleClient);

	if (WaitForSingleObject(handleServer, 10000) != WAIT_OBJECT_0)
		TerminateThread(handleServer, 0);
	CloseHandle(handleServer);

	printf("======================\ndone");
	getchar();
	return 0;
}