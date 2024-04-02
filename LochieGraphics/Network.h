#pragma once


#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib") // TODO: link properly

//TODO: Maybe this should be a super class of client and server
class Network
{
public:
	static bool CommonSetup(std::string ip, std::string port, addrinfo** info, SOCKET* soc);
	static bool SetSocketNonBlocking(SOCKET* soc, addrinfo** info);
	static bool Send(SOCKET soc, const char* sendBuffer);

private:
	static ULONG uNonBlockingMode;

	static bool InitWinsock();
	static bool GetAddressInfo(std::string ip, std::string port, addrinfo* hints, addrinfo** info);
	static bool CreateSocket(SOCKET* soc, addrinfo** info);
};
