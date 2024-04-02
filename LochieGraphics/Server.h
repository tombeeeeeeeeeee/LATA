#pragma once

#include "Network.h"

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

#define DEFAULT_CLIENT_LIMIT 4

typedef void(*RecieveCallback)(const char* buffer);


class Server
{
public:
	void Start();
	void Run();
	void CheckConnectClient(); //TODO: prob could just be called in the run loop, but keep seperate maybe cause not always wanna check for clients

	void Close();

	void Send(int clientIndex, const char* sendBuf);
	void Broadcast(const char* sendBuf);

	RecieveCallback recieveCallback = nullptr;

	// TODO: rename with lowercases
	SOCKET ListenSocket = INVALID_SOCKET; // TODO: Not listen after client limit reached?
	SOCKET ClientSocket[DEFAULT_CLIENT_LIMIT] = { INVALID_SOCKET, INVALID_SOCKET, INVALID_SOCKET, INVALID_SOCKET };

	std::string address;
	std::string port;

	
	bool started = false;

private:
	char recvbuf[DEFAULT_CLIENT_LIMIT][DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	ULONG uNonBlockingMode = 1;
	// TODO: Connecting should no longer be blocking and should be able to happen all on a single thread
	void CloseClient(int i);
};

