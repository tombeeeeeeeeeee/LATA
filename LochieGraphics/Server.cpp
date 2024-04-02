#include "Server.h"

void Server::Start()
{
	std::cout << "Starting Server...\n";

	addrinfo* info = nullptr;
	
	if (!CommonSetup(&info, &ListenSocket))

	if (!SetSocketNonBlocking(&ListenSocket, &info)) { return; }

	int iResult;
	// Setup the TCP listening socket; bind
	iResult = bind(ListenSocket, info->ai_addr, (int)info->ai_addrlen);
	freeaddrinfo(info);
	if (iResult == SOCKET_ERROR) {
		std::cout << "bind failed with error: " << WSAGetLastError() << "\n";
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}

	// Listen
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		std::cout << "Listen failed with error: " << WSAGetLastError() << "\n";
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}

	started = true;
}

void Server::Run()
{
	int iResult;

	for (int i = 0; i < DEFAULT_CLIENT_LIMIT; i++)
	{
		if (ClientSocket[i] == INVALID_SOCKET) { continue; }

		WSAPOLLFD fdArray = { 0 };
		fdArray.events = POLLRDNORM;
		fdArray.fd = ClientSocket[i];
		// WSAPoll (array, array elements, how long to wait (0 for no wait return immediately))
		if (WSAPoll(&fdArray, 1, 0) == 0) { continue; }

		iResult = recv(ClientSocket[i], recvbuf[i], recvbuflen, 0);
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
		if (ClientSocket[i] != INVALID_SOCKET) { continue; }
		// Accept a client socket

		WSAPOLLFD fdArray = { 0 };
		fdArray.events = POLLRDNORM;
		fdArray.fd = ListenSocket;

		int check = WSAPoll(&fdArray, 1, 0);

		if (check == 0) {
			return; //TODO:
		}
		std::cout << "New client connecting...: " << "\n";

		ClientSocket[i] = accept(ListenSocket, NULL, NULL);
		if (ClientSocket[i] == INVALID_SOCKET) {
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
	int iResult = shutdown(ClientSocket[i], SD_SEND);
	closesocket(ClientSocket[i]);
	if (iResult == SOCKET_ERROR) {
		std::cout << "shutdown failed: " << WSAGetLastError() << "\n";
		return;
	}

	// cleanup
	ClientSocket[i] = INVALID_SOCKET;
}

void Server::Close()
{
	WSACleanup();
}

void Server::Send(int clientIndex, const char* sendBuf)
{
	Network::Send(ClientSocket[clientIndex], sendBuf);
}

void Server::Broadcast(const char* sendBuf)
{
	for (int i = 0; i < DEFAULT_CLIENT_LIMIT; i++)
	{
		if (ClientSocket[i] == INVALID_SOCKET) { continue; }
		Send(i, sendBuf);
	}
}
