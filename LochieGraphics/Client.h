#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512

typedef void(*RecieveCallback)(const char* buffer);

//TODO: Error when cannot join
class Client
{
private:
	SOCKET ConnectSocket = INVALID_SOCKET;
	ULONG uNonBlockingMode = 1;


public:
	RecieveCallback recieveCallback = nullptr;
	int Start();
	int Run();
	int Send(const char* sendBuf);
	int Disconnect();
	int Close();

public:
	std::string address;
	std::string port;
};

