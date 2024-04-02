#pragma once
#include "Messenger.h"

#include "Client.h"

#include <vector>

class MessengerClient : public Messenger
{
	Client client;

	static void RecieveMessage(const char* message);
public:

	void Start(std::string add, std::string port) override;
	void Update() override;
	void Close() override;

	void Send(const char* message) override;

	
};

