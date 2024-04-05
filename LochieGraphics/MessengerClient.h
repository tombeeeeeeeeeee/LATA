#pragma once
#include "MessengerNetwork.h"

#include "Client.h"

class MessengerClient : public MessengerNetwork
{
	Client client;

	static void RecieveMessage(const char* message);
public:

	void Start(std::string add, std::string port) override;
	void Update() override;
	void Close() override;

	void Send(const char* message) override;

	
};

