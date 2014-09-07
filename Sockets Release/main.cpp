#pragma region Memory leaks tracing
#if (defined(_WIN32) || defined(_WIN64)) && defined(_DEBUG)
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif

#if (defined(_WIN32) || defined(_WIN64)) && defined(_DEBUG)
	#ifdef _DEBUG
		#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
		#define new DEBUG_NEW
	#endif
#endif
#pragma endregion

#include "../Connection Library/host.h"
#pragma comment(lib, "Connection Library")

void* ClientThread(void*)
{
	Host client;

	if (client.Connect("localhost", 0xff))
	{
		printf("Client | connected to %s on %i\n", client.GetIPAddress().c_str(), client.GetPort());
		printf("Client | sent: %i bytes\n", client.Send("Data from client"));
		printf("Client | read: %s\n", client.Receive().c_str());
		printf("Client | sent: %i bytes\n", client.Send("Last message from client"));
		printf("Client | read: %s\n", client.Receive().c_str());
		printf("Client | file: %s\n", client.SendFile("in.log") ? "good" : "bad");

		if (client.Disconnect()) puts("Client | connection closed");
		else puts("Client | cannot close connection");
	}
	else puts("Client | cannot connect");

	return NULL;
}

void* ServerThread(void*)
{
	Host server;
	std::string file = "out.log";

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

	return NULL;
}

int main()
{
#if (defined(_WIN32) || defined(_WIN64)) && defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

#if (defined(_WIN32) || defined(_WIN64))
	/* Running threads on Windows */
	HANDLE handleServer = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ServerThread, NULL, NULL, NULL);
	Sleep(1000);
	HANDLE handleClient = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ClientThread, NULL, NULL, NULL);

	if (WaitForSingleObject(handleClient, 10000) != WAIT_OBJECT_0)
		TerminateThread(handleClient, 0);
	CloseHandle(handleClient);

	if (WaitForSingleObject(handleServer, 10000) != WAIT_OBJECT_0)
		TerminateThread(handleServer, 0);
	CloseHandle(handleServer);
#else
	/* Running threads on Linux */
	pthread_t client, server;

	pthread_create(&server, NULL, &ServerThread, NULL);
	sleep(1);
	pthread_create(&client, NULL, &ClientThread, NULL);

	pthread_join(client, NULL);
	pthread_join(server, NULL);
#endif

	printf("======================\ndone");
	return 0;
}