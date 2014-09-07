libDir := "../Connection Library/host.cpp"
mainDir := "../Sockets Release/main.cpp"
date=$(shell date +"%d-%m-%Y / %T")
line=~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

all:
	$(info ${line})
	$(info Sockets Connection Library | Available options: gcc, mingw)
	$(info ${line})
	$(info ${date}                           by lowliet)
	@-exit 0

gcc: lib
	g++ ${mainDir} -L. -lhost -lpthread -O2 -o Release
	rm libhost.*
	mkdir -p ../Release
	mv Release "../Release/"
	cp ${libDir} ../Release/in.log

mingw: lib
	g++ ${mainDir} -L. -lhost -lws2_32 -O2 -o Release
	rm libhost.*
	mkdir -p ../Release
	mv Release.exe ../Release/
	cp ${libDir} ../Release/in.log

lib:
	g++ -c ${libDir} -o libhost.o
	ar rcs libhost.a libhost.o