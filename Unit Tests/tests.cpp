#include "../Connection Library/host.h"
#include "WinUnit\Include\WinUnit.h"

#pragma region Thread Functions
void ClientThread(LPVOID lpParam)
{
	Host client;

	client.Connect("localhost", 0xff);

	unsigned int count = 1000;
	while (count --> 0)
	{
		std::string send(64, '\0');
		sprintf((char*) send.c_str(), "%i%i%i", rand(), rand(), rand());

		client.Send(send);
		client.Receive();

		sprintf((char*) send.c_str(), "%08i%08i%08i", rand(), rand(), rand());

		client.Send(send);
		client.Receive();
	}

	client.Disconnect();
}

void ClientThreadFile(LPVOID lpParam)
{
	Host client;

	client.Connect("localhost", 0xff);

	FILE *file = fopen("test_send_file.txt", "w");

	if (file != NULL)
	{
		for (unsigned int i = 0; i < 4096; ++i)
			fprintf(file, "\t%i |\t%i |\t%i\n", i, i * 2, i * 4);
		fclose(file);
	}

	unsigned int count = 1000;
	while (count--> 0)
	{
		client.SendFile("test_send_file.txt");
	}

	DeleteFile("test_send_file.txt");

	client.Disconnect();
}

void ServerThread(LPVOID lpParam)
{
	Host server;

	if (server.Listen(0xff))
	{
		Host *connectedHost = server.Accept();

		unsigned int count = 1000;
		while (count --> 0)
		{
			std::string receive = connectedHost->Receive();
			connectedHost->Send(receive);

			std::string receiveVec = connectedHost->Receive(32, false);
			connectedHost->Send(receiveVec.data(), receiveVec.size());
		}
	
		connectedHost->Disconnect();
		server.Disconnect();
		delete connectedHost;
	}
}

void ServerThreadFile(LPVOID lpParam)
{
	Host server;

	if (server.Listen(0xff))
	{
		Host *connectedHost = server.Accept();

		unsigned int count = 1000;
		while (count-- > 0)
		{
			std::string fileName = "receive_test_file.txt";
			connectedHost->ReceiveFile(fileName);
			DeleteFile(fileName.c_str());
		}

		connectedHost->Disconnect();
		server.Disconnect();
		delete connectedHost;
	}
}
#pragma endregion

BEGIN_TEST(Listening) {
	Host host;
	
	WIN_ASSERT_FALSE(host.Listen(0));
	WIN_ASSERT_TRUE(host.Listen(100));
	WIN_ASSERT_FALSE(host.Listen(100));
} END_TEST

BEGIN_TEST(Connecting) {
	Host client, server;

	WIN_ASSERT_FALSE(client.Connect("", 100));
	WIN_ASSERT_FALSE(client.Connect("thisisnotvalidaddress", 100));
	WIN_ASSERT_FALSE(client.Connect("localhost", 10));

	server.Listen(0xff);

	unsigned int count = 10;
	while (count --> 0)
	{
		WIN_ASSERT_TRUE(client.Connect("localhost", 0xff));
		WIN_ASSERT_TRUE(client.Disconnect());
	}
} END_TEST

BEGIN_TEST(Disconnecting) {
	Host host;

	WIN_ASSERT_FALSE(host.Disconnect());
} END_TEST

BEGIN_TEST(ReceivingData) {
	DWORD threadCode = 0;
	srand((unsigned int)time(0));

	Host server;

	WIN_ASSERT_TRUE(server.Listen(0xff));

	HANDLE handleClient = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ClientThread, 0, 0, &threadCode);

	Host *connectedHost = server.Accept();
	WIN_ASSERT_NOT_NULL(connectedHost);

	WIN_ASSERT_STRING_EQUAL("127.0.0.1", connectedHost->GetIPAddress().c_str());
	WIN_ASSERT_EQUAL(0xff, connectedHost->GetPort());

	unsigned int count = 1000;
	while (count --> 0)
	{
		std::string receive = connectedHost->Receive();
		WIN_ASSERT_FALSE(receive.empty());
		WIN_ASSERT_EQUAL(strlen(receive.c_str()), connectedHost->Send(receive));

		receive = connectedHost->Receive(24);
		WIN_ASSERT_FALSE(receive.empty());
		WIN_ASSERT_TRUE(strlen(receive.c_str()) == 24);
		WIN_ASSERT_TRUE(connectedHost->Send(receive));
	}
	
	WIN_ASSERT_TRUE(connectedHost->Disconnect());
	WIN_ASSERT_TRUE(server.Disconnect());
	delete connectedHost;

	WIN_ASSERT_TRUE(WaitForSingleObject(handleClient, 10000) == WAIT_OBJECT_0);
	WIN_ASSERT_TRUE(CloseHandle(handleClient));
} END_TEST

BEGIN_TEST(SendingData) {
	DWORD threadCode = 0;
	srand((unsigned int) time(0));

	HANDLE handleServer = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ServerThread, 0, 0, &threadCode);

	Host client;

	Sleep(1000);
	WIN_ASSERT_TRUE(client.Connect("localhost", 0xff));

	WIN_ASSERT_STRING_EQUAL("127.0.0.1", client.GetIPAddress().c_str());
	WIN_ASSERT_EQUAL(0xff, client.GetPort());

	unsigned int count = 1000;
	while (count --> 0)
	{
		std::string send(64, '\0');
		sprintf((char*) send.c_str(), "%i%i%i", rand(), rand(), rand());

		WIN_ASSERT_EQUAL(strlen(send.c_str()), client.Send(send));
		WIN_ASSERT_STRING_EQUAL(send.c_str(), client.Receive().c_str());

		char buffer [] = "example\0\0\0binary\t\ndata";
		char receive[32];

		WIN_ASSERT_EQUAL(sizeof(buffer), client.Send(buffer, sizeof(buffer)));
		WIN_ASSERT_EQUAL(sizeof(buffer), client.Receive(receive, sizeof(buffer), true));

		for (unsigned int i = 0; i < sizeof(buffer); ++i)
			WIN_ASSERT_TRUE(buffer[i] == receive[i]);
	}

	WIN_ASSERT_TRUE(client.Disconnect());
	WIN_ASSERT_TRUE(WaitForSingleObject(handleServer, 10000) == WAIT_OBJECT_0);
	WIN_ASSERT_TRUE(CloseHandle(handleServer));
} END_TEST

BEGIN_TEST(ReceivingFile) {
	DWORD threadCode = 0;
	srand((unsigned int) time(0));

	Host server;

	WIN_ASSERT_TRUE(server.Listen(0xff));

	HANDLE handleClient = CreateThread(0, 0, (LPTHREAD_START_ROUTINE) ClientThreadFile, 0, 0, &threadCode);

	Host *connectedHost = server.Accept();
	WIN_ASSERT_NOT_NULL(connectedHost);

	WIN_ASSERT_STRING_EQUAL("127.0.0.1", connectedHost->GetIPAddress().c_str());
	WIN_ASSERT_EQUAL(0xff, connectedHost->GetPort());

	unsigned int count = 1000;
	while (count --> 0)
	{
		std::string fileName = "receive_test_file.txt";
		WIN_ASSERT_TRUE(connectedHost->ReceiveFile(fileName));

		FILE *file = fopen(fileName.c_str(), "r");
		unsigned int reference = 0;
		WIN_ASSERT_NOT_NULL(file);
		while (!feof(file))
		{
			int a, b, c;
			if (fscanf(file, "\t%i |\t%i |\t%i\n", &a, &b, &c) <= 0) break;
			WIN_ASSERT_EQUAL(reference, a);
			WIN_ASSERT_EQUAL(reference * 2, b);
			WIN_ASSERT_EQUAL(reference * 4, c);
			++reference;
		}
		fclose(file);
		WIN_ASSERT_TRUE(DeleteFile(fileName.c_str()));
	}

	WIN_ASSERT_TRUE(connectedHost->Disconnect());
	WIN_ASSERT_TRUE(server.Disconnect());
	delete connectedHost;

	WIN_ASSERT_TRUE(WaitForSingleObject(handleClient, 10000) == WAIT_OBJECT_0);
	WIN_ASSERT_TRUE(CloseHandle(handleClient));
} END_TEST

BEGIN_TEST(SendingFile) {
	DWORD threadCode = 0;
	srand((unsigned int) time(0));

	HANDLE handleServer = CreateThread(0, 0, (LPTHREAD_START_ROUTINE) ServerThreadFile, 0, 0, &threadCode);

	Host client;

	Sleep(1000);
	WIN_ASSERT_TRUE(client.Connect("localhost", 0xff));

	WIN_ASSERT_STRING_EQUAL("127.0.0.1", client.GetIPAddress().c_str());
	WIN_ASSERT_EQUAL(0xff, client.GetPort());

	FILE *file = fopen("test_send_file.txt", "w");

	WIN_ASSERT_NOT_NULL(file);

	for (unsigned int i = 0; i < 4096; ++i)
		fprintf(file, "\t%i |\t%i |\t%i\n", i, i * 2, i * 4);
	fclose(file);

	unsigned int count = 1000;
	while (count --> 0)
	{
		WIN_ASSERT_TRUE(client.SendFile("test_send_file.txt"));
	}

	WIN_ASSERT_TRUE(DeleteFile("test_send_file.txt"));

	WIN_ASSERT_TRUE(client.Disconnect());
	WIN_ASSERT_TRUE(WaitForSingleObject(handleServer, 10000) == WAIT_OBJECT_0);
	WIN_ASSERT_TRUE(CloseHandle(handleServer));
} END_TEST