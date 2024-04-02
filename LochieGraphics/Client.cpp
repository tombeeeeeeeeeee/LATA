#include "Client.h"

void Client::Start()
{
	std::cout << "Starting Client...\n";

	addrinfo* info = nullptr;

	if (!Network::CommonSetup(address, port, &info, &ConnectSocket)) { return; }

	int iResult;
	// Connect to server.
	iResult = connect(ConnectSocket, info->ai_addr, (int)info->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
		std::cout << "Unable to connect to server: " << WSAGetLastError() << "\n";
		WSACleanup();
		return;
	}

	if (!Network::SetSocketNonBlocking(&ConnectSocket, &info)) { return; }

	freeaddrinfo(info);
}

void Client::Run() // check if gonna block
{
	int iResult;

	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	WSAPOLLFD fdArray = { 0 };
	fdArray.events = POLLRDNORM;
	fdArray.fd = ConnectSocket;

	int check = WSAPoll(&fdArray, 1, 0);

	if (check == 0) {
		return;
	}


	iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
	if (iResult > 0) {
		std::cout << "Bytes received: " << iResult << "\n";

		if (recieveCallback != nullptr) {
			recieveCallback(recvbuf);
		}
	}
	else if (iResult == 0) {
		std::cout << "Connection closed\n";
		Disconnect();
	}
	else {
		std::cout << "recv failed: " << WSAGetLastError() << "\n";
	}

}

void Client::Send(const char* sendBuf)
{
	Network::Send(ConnectSocket, sendBuf);
}

void Client::Disconnect()
{
	// the client can still use the ConnectSocket for receiving data
	int iResult = shutdown(ConnectSocket, SD_SEND);
	closesocket(ConnectSocket);
	if (iResult == SOCKET_ERROR) {
		std::cout << "shutdown failed: " << WSAGetLastError() << "\n";
		WSACleanup();
		return;
	}
}

void Client::Close()
{
	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();
}
