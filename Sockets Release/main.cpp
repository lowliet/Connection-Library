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
	std::vector<char> data;

	if (host.Connect("localhost", 9000))
	{
		puts("connected");

		printf("Sended: %i bytes\n", host.Send("This is test"));
		data = host.Receive(32);
		printf("Readed: %i bytes\n", data.size());

		for (unsigned int i = 0; i < data.size(); i++)
			printf("%c", data[i]);
		puts("");

		if (host.Disconnect())
			puts("connection closed");
		else
			puts("cannot close connection");
	}
	else
		puts("cannot connect");

	puts("done");
	getchar();
	return 0;
}