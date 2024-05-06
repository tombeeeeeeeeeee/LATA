#include "MessengerClient.h"

void MessengerClient::RecieveMessage(const char* message)
{
	messages.push_back(message);
}

bool MessengerClient::Start(std::string add, std::string port)
{
	client.address = add;
	client.port = port;
	client.recieveCallback = RecieveMessage;
	return client.Start();
}

void MessengerClient::Update()
{
	client.Run();
}

void MessengerClient::Close()
{
	client.Disconnect();
	client.Close();
}

void MessengerClient::Send(const char* message)
{
	client.Send(message);
}
