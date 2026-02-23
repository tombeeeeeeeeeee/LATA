#pragma once
#include "MessengerNetwork.h"

#include "Server.h"

class MessengerServer : public MessengerNetwork
{
	static Server server;

	bool Start(std::string add, std::string port) override;

	void Update() override;

	void Close() override;

	void Send(const char* message) override;

	static void RecieveMessage(const char* message);
};

