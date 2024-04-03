#pragma once


#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib") // TODO: link properly

#define DEFAULT_BUFLEN 512

#define DEFAULT_CLIENT_LIMIT 4

typedef void(*RecieveCallback)(const char* buffer);

//TODO: Maybe this should be a super class of client and server
class Network
{
public:

	void virtual Start() = 0;
	void virtual Run() = 0;
	void virtual Close() = 0;

	std::string address;
	std::string port;

	RecieveCallback recieveCallback = nullptr;

protected:
	bool CommonSetup(addrinfo** info, SOCKET* soc);
	static bool SetSocketNonBlocking(SOCKET* soc, addrinfo** info);
	static bool Send(SOCKET soc, const char* sendBuffer);

	// Readable and writable are mean that the socket will be non blocking
	static bool SocketReadable(SOCKET* soc);
	static bool SocketWritable(SOCKET* soc);
private:
	static ULONG uNonBlockingMode;

	static bool InitWinsock();
	static bool GetAddressInfo(std::string ip, std::string port, addrinfo* hints, addrinfo** info);
	static bool CreateSocket(SOCKET* soc, addrinfo** info);
};
