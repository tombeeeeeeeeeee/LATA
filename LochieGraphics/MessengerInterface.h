#pragma once

#include "MessengerNetwork.h"

class MessengerInterface
{
	bool serverTicked = false;

	bool started = false;

	MessengerNetwork* messenger;
	
	const int maxMessageLength = 256;
	char typedMessage[256];
	
	// TODO: better types
	const int maxAddressLength = 15;
	char address[15] = "127.0.0.1";
	int port = 7777;


	void Start();
public:
	void GUI() override;

	void Update() override;
};