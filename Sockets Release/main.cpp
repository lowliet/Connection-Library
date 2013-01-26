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

int main()
{
	#if (defined(_WIN32) || defined(_WIN64)) && defined(_DEBUG)
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	#endif

	Host host;

	if (host.Connect("localhost", 9000))
	{
		printf("connected to %s on %i\n", host.GetIPAddress().c_str(), host.GetPort());

		printf("Sended: %i bytes\n", host.Send("This is test"));
		std::vector<char> data = host.Receive(32);
		printf("Readed: %i bytes\n", data.size());

		for (unsigned int i = 0; i < data.size(); i++) printf("%c", data[i]);
		puts("");

		if (host.SendFile("main.cpp")) puts("File sended");
		else puts("Cannot send file");

		if (host.Listen(123)) puts("listening...");
		else puts("cannot listen");

		data.push_back('\n');
		Host *client = host.Accept();
		printf("connected to %s on %i\n", client->GetIPAddress().c_str(), client->GetPort());
		printf("Sended: %i bytes\n", client->Send(data));
		data = client->Receive(32);
		printf("Readed: %i bytes\n", data.size());
		for (unsigned int i = 0; i < data.size(); i++) printf("%c", data[i]);
		puts("");
		Host::receiveStruct rec = client->Receive();
		if (rec.isInitialized) printf("Received int: %i\n", rec.Int);
		else puts("Cannot receive");
		delete client;

		if (host.Disconnect()) puts("connection closed");
		else puts("cannot close connection");
	}
	else puts("cannot connect");

	puts("done");
	getchar();
	return 0;
}