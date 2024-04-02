#pragma once

#include "Network.h"


//TODO: Error when cannot join
class Client : public Network
{
private:
	SOCKET ConnectSocket = INVALID_SOCKET;

public:
	void Start() override;
	void Run() override;
	void Send(const char* sendBuf);
	void Disconnect();
	void Close() override;
};

