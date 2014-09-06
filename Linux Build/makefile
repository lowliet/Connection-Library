all:
	g++ -c "../Connection Library/host.cpp" -o libhost.o
	ar rcs libhost.a libhost.o
	g++ "../Sockets Release/main.cpp" -L. -lhost -lpthread -O2 -o Release
	rm libhost.*
	mv Release "../Release/"

mingw:
	g++ -c "../Connection Library/host.cpp" -o libhost.o
	ar rcs libhost.a libhost.o
	g++ "../Sockets Release/main.cpp" -L. -lhost -lws2_32 -O2 -o Release
	rm libhost.*
	mv Release.exe ../Release/