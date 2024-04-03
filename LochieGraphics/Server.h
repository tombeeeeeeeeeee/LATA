#pragma once

#include "Network.h"

typedef void(*RecieveCallback)(const char* buffer);


class Server : public Network
{
public:
	void Start() override;
	void Run() override;
	void CheckConnectClient(); //TODO: prob could just be called in the run loop, but keep seperate maybe cause not always wanna check for clients

	void Close() override;

	void Send(int clientIndex, const char* sendBuf);
	void Broadcast(const char* sendBuf);


	// TODO: rename with lowercases
	SOCKET listenSocket = INVALID_SOCKET; // TODO: Not listen after client limit reached?
	SOCKET clientSockets[DEFAULT_CLIENT_LIMIT] = { INVALID_SOCKET, INVALID_SOCKET, INVALID_SOCKET, INVALID_SOCKET };
	
	bool started = false; // TODO: maybe actually use

private:
	char recvbuf[DEFAULT_CLIENT_LIMIT][DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	// TODO: Connecting should no longer be blocking and should be able to happen all on a single thread
	void CloseClient(int i);

	static bool BindSocket(SOCKET* soc, addrinfo** info, std::string port);
	static bool ListenOnSocket(SOCKET* soc);
};

