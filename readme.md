Sockets Connection Library
=========================

Simple wrapper providing primitives for exchanging data via TCP Sockets on Windows and Linux

--------------

## Building

 - **Windows** - Open solution file using `Visual Studio 2013` and issue build
 - **Linux** - Browse to `Linux Build` folder and run `make`

--------------

## Documentation

Run `Doxygen` in `Documentation/Config`, then open `index.html` from `Documentation/Documentation`

--------------

## Content

 - **Connection Library** - socket library
 - **Documentation** - [Doxygen](http://doxygen.org) based documentation
 - **Socket Release** - example application
 - **Unit Tests** - unit tests based on [WinUnit](http://winunit.codeplex.com/)

--------------

## Example code

    Host client;

    if (client.Connect("localhost", 255))
    {
        printf("Connected to %s\n", client.GetIPAddress().c_str());
        printf("Sent: %i bytes\n", client.Send("Data from client"));
        printf("Read: %s\n", client.Receive().c_str());

        client.Disconnect();
    }

--------------