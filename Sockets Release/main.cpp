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

	if (host.Connect("onet.pl", 80))
	{
		puts("connected");
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