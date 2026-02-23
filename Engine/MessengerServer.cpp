#include "MessengerServer.h"

Server MessengerServer::server;

bool MessengerServer::Start(std::string add, std::string port)
{
	server.address = add;
	server.port = port;
	server.recieveCallback = RecieveMessage;
	return server.Start();
}

void MessengerServer::Update()
{
	server.CheckConnectClient();
	server.Run();
}

void MessengerServer::Close()
{
	server.Close();
}

void MessengerServer::Send(const char* message)
{
	server.Broadcast(message);
	messages.push_back(message);
}

void MessengerServer::RecieveMessage(const char* message)
{
	server.Broadcast(message);
	messages.push_back(message);
}
