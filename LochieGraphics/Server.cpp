#include "Server.h"

int Server::Start()
{
	std::cout << "Starting Server...\n";

	WSADATA wsaData;

	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); // Makeword is for specifying the version
	if (iResult != 0) {
		std::cout << "WSAStartup failed: " << iResult << "\n";
		return -1;
	}
	else {
		std::cout << "WSAStartup succeeded!\n";
	}

	addrinfo* result = NULL, * ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	iResult = getaddrinfo(address.c_str(), port.c_str(), &hints, &result);
	if (iResult != 0) {
		std::cout << "getaddrinfo failed: " << iResult << "\n";
		WSACleanup();
		return -1;
	}
	else {
		std::cout << "Address and port resolved!\n";
	}

	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		std::cout << "Error creating Listen Socket: " << WSAGetLastError() << "\n";
		freeaddrinfo(result);
		WSACleanup();
		return -1;
	}
	else {
		std::cout << "Socket made!\n";
	}

	// Set socket to non blocking mode
	iResult = ioctlsocket(ListenSocket, FIONBIO, &uNonBlockingMode);
	if (iResult == SOCKET_ERROR) {
		std::cout << "Couldn't set socket mode: " << WSAGetLastError() << "\n";
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return -1;
	}



	// Setup the TCP listening socket; bind
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		std::cout << "bind failed with error: " << WSAGetLastError() << "\n";
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return -1;
	}

	freeaddrinfo(result);

	// Listen
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		std::cout << "Listen failed with error: " << WSAGetLastError() << "\n";
		closesocket(ListenSocket);
		WSACleanup();
		return -1;
	}

	return 0;
}

int Server::Run()
{
	int iResult;

	for (int i = 0; i < DEFAULT_CLIENT_LIMIT; i++)
	{
		if (ClientSocket[i] == INVALID_SOCKET) { continue; }
		WSAPOLLFD fdArray = { 0 };

		fdArray.events = POLLRDNORM;
		fdArray.fd = ClientSocket[i];

		// WSAPoll (array, array elements, how long to wait (0 for no wait return immediately))
		int check = WSAPoll(&fdArray, 1, 0);
		if (check == 0) { continue; }
		//std::cout << "Check made it!\n";
		//continue;

		iResult = recv(ClientSocket[i], recvbuf[i], recvbuflen, 0);
		if (iResult > 0) {
			std::cout << "Bytes received: " << iResult << "\n";
			std::cout << recvbuf[i] << "\n";

			// TODO: Callback here
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
			CloseClient(i);
			return -1;
		}
	}
	return 0;
}

int Server::CheckConnectClient()
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
			return -1; //TODO:
		}
		std::cout << "New client connecting...: " << "\n";

		ClientSocket[i] = accept(ListenSocket, NULL, NULL);
		if (ClientSocket[i] == INVALID_SOCKET) {
			std::cout << "accept failed: " << WSAGetLastError() << "\n";
			return -1;
		}
		else {
			std::cout << "Client accepted!\n";
		}

		if (i < DEFAULT_CLIENT_LIMIT) {
			CheckConnectClient();
		}
		else {
			std::cout << "Client Limit Reached!\n";
		}
	}
	return 0;
}

void Server::CloseClient(int i)
{
	int iResult = shutdown(ClientSocket[i], SD_SEND);
	if (iResult == SOCKET_ERROR) {
		std::cout << "shutdown failed: " << WSAGetLastError() << "\n";
		closesocket(ClientSocket[i]);
		return;
	}
	else {
		std::cout << "Shutdown client" << i << "!\n";
	}

	// cleanup
	closesocket(ClientSocket[i]);
	ClientSocket[i] = INVALID_SOCKET;
}

void Server::Close()
{
	WSACleanup();
}

void Server::Send(int clientIndex, const char* sendBuf)
{
	int iSendResult;

	// TODO: Make sure send will be non blocking
	iSendResult = send(ClientSocket[clientIndex], sendBuf, strlen(sendBuf) + 1, 0);
	if (iSendResult == SOCKET_ERROR) {
		std::cout << "send failed: " << WSAGetLastError() << "\n";
		//closesocket(ClientSocket[clientIndex]);
		return;
	}
	std::cout << "Bytes sent: " << iSendResult << "\n";
}

void Server::Broadcast(const char* sendBuf)
{
	for (int i = 0; i < DEFAULT_CLIENT_LIMIT; i++)
	{
		if (ClientSocket[i] == INVALID_SOCKET) { continue; }
		Send(i, sendBuf);
	}
}
