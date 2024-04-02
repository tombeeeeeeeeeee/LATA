#pragma once
#include "Messenger.h"

#include "Server.h"

#include <vector>

class MessengerServer : public Messenger
{
	static Server server;

	void Start(std::string add, std::string port) override;

	void Update() override;

	void Close() override;

	void Send(const char* message) override;

	static void RecieveMessage(const char* message);
};

