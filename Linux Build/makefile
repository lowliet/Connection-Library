all:
	g++ -c "../Connection Library/host.cpp" -o libhost.o
	ar rcs libhost.a libhost.o
	g++ "../Sockets Release/main.cpp" -L. -lhost -lws2_32 -o release
	del /Q libhost.*

linux:
	g++ -c "../Connection Library/host.cpp" -o libhost.o
	ar rcs libhost.a libhost.o
	g++ "../Sockets Release/main.cpp" -L. -lhost -o release
	rm libhost.*