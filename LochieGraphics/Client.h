#pragma once

#include "Network.h"

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
	void Start();
	void Run();
	void Send(const char* sendBuf);
	void Disconnect();
	void Close();

public:
	std::string address;
	std::string port;
};

