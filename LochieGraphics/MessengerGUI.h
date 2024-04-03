#pragma once

#include "Messenger.h"

// TODO: Bad class name, this owns the server and client stuff
class MessengerGUI
{
	bool serverTicked = false;

	bool started = false;

	Messenger* messenger;
	
	const int bufSize = 256;
	char messageBuffer[256];
	
	// TODO: better types
	const int maxAddressLength = 15;
	char address[15] = "127.0.0.1";
	int port = 7777;


	void Start();
public:
	void GUI();

	void Update();
	
};

