#if (defined(_WIN32) || defined(_WIN64)) && defined(_DEBUG)
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif

#include "../Connection Library/host.h"

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
		printf("connected to %s\n", host.GetIPAddress().c_str());

		printf("Sended: %i bytes\n", host.Send("This is test"));
		std::vector<char> data = host.Receive(32);
		printf("Readed: %i bytes\n", data.size());

		for (unsigned int i = 0; i < data.size(); i++) printf("%c", data[i]);
		puts("");

		if (host.Listen(123)) puts("listening...");
		else puts("cannot listen");

		Host *con = host.Accept();
		printf("connected to %s\n", con->GetIPAddress().c_str());
		printf("Sended: %i bytes\n", con->Send("Connected to client...\n"));
		data = con->Receive(32);
		printf("Readed: %i bytes\n", data.size());
		for (unsigned int i = 0; i < data.size(); i++) printf("%c", data[i]);
		puts("");
		delete con;

		if (host.Disconnect()) puts("connection closed");
		else puts("cannot close connection");
	}
	else puts("cannot connect");

	puts("done");
	getchar();
	return 0;
}