#include "Server.h"

#include<string>


bool Server::Start()
{
	std::cout << "Starting Server...\n";

	addrinfo* info = nullptr;
	
	if (!CommonSetup(&info, &listenSocket))

	if (!SetSocketNonBlocking(&listenSocket, &info)) { return false; }
	
	// Bind listening socket to address
	if (!BindSocket(&listenSocket, &info, port)) { return false; };

	freeaddrinfo(info);

	// Listen
	if (!ListenOnSocket(&listenSocket)) { return false; }
	
	started = true;
	return true;
}

void Server::Run()
{
	for (int i = 0; i < DEFAULT_CLIENT_LIMIT; i++)
	{
		if (clientSockets[i] == INVALID_SOCKET) { continue; }

		if (!SocketReadable(&clientSockets[i])) { continue; }

		int iResult = recv(clientSockets[i], recvbuf[i], recvbuflen, 0);
		if (iResult > 0) {
			std::cout << "Bytes received: " << iResult << "\n";
			std::cout << recvbuf[i] << "\n";

			if (recieveCallback != nullptr) {
				recieveCallback(recvbuf[i]);
			}
		}
		else if (iResult == 0) {
			std::cout << "Connection closing...\n";
			CloseClient(i);
		}
		else {
			std::cout << "recv failed: " << WSAGetLastError() << "\n"; //TODO: if the client dissconnected error code is here remove that client
			CloseClient(i); // TODO: maybe don't always just close the client
		}
	}
}

void Server::CheckConnectClient()
{
	// Find avaliable client spot
	for (int i = 0; i < DEFAULT_CLIENT_LIMIT; i++)
	{
		// Client slot already being used
		if (clientSockets[i] != INVALID_SOCKET) { continue; }

		// If reading will block, return early
		if (!SocketReadable(&listenSocket)) {
			return;
		}
		
		std::cout << "New client connecting...: " << "\n";

		// Accept a client socket
		clientSockets[i] = accept(listenSocket, NULL, NULL);
		if (clientSockets[i] == INVALID_SOCKET) {
			std::cout << "accept failed: " << WSAGetLastError() << "\n";
			return;
		}
		else {
			std::cout << "Client accepted!\n";
		}
	}
}

void Server::CloseClient(int i)
{
	int error = shutdown(clientSockets[i], SD_SEND);
	closesocket(clientSockets[i]);
	if (error == SOCKET_ERROR) {
		std::cout << "shutdown failed: " << WSAGetLastError() << "\n";
		return;
	}

	// cleanup
	clientSockets[i] = INVALID_SOCKET;
}

bool Server::BindSocket(SOCKET* soc, addrinfo** info, std::string port)
{
	sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(std::stoi(port));
	server_address.sin_addr.S_un.S_addr = INADDR_ANY; // Bind to no particular address (any)

	int error = bind(*soc, (sockaddr*)&server_address, sizeof(server_address));
	if (error == SOCKET_ERROR) {
		std::cout << "bind failed with error: " << WSAGetLastError() << "\n";
		closesocket(*soc);
		WSACleanup();
		return false;
	}
	return true;
}

bool Server::ListenOnSocket(SOCKET* soc)
{
	if (listen(*soc, SOMAXCONN) == SOCKET_ERROR) {
		std::cout << "Listen failed with error: " << WSAGetLastError() << "\n";
		closesocket(*soc);
		WSACleanup();
		return false;
	}
	return true;
}

void Server::Close()
{
	WSACleanup();
}

void Server::Send(int clientIndex, const char* sendBuf)
{
	Network::Send(clientSockets[clientIndex], sendBuf);
}

void Server::Broadcast(const char* sendBuf)
{
	for (int i = 0; i < DEFAULT_CLIENT_LIMIT; i++)
	{
		if (clientSockets[i] == INVALID_SOCKET) { continue; }
		Send(i, sendBuf);
	}
}
