#include "Client.h"

bool Client::Start()
{
	std::cout << "Starting Client...\n";

	addrinfo* info = nullptr;

	if (!CommonSetup(&info, &connectSocket)) { return false; }

	int iResult;
	// Connect to server.
	iResult = connect(connectSocket, info->ai_addr, (int)info->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(connectSocket);
		connectSocket = INVALID_SOCKET;
		std::cout << "Unable to connect to server: " << WSAGetLastError() << "\n";
		WSACleanup();
		return false;
	}

	if (!SetSocketNonBlocking(&connectSocket, &info)) { return false; }

	freeaddrinfo(info);
	return true;
}

void Client::Run()
{
	int iResult;

	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	// Check if it is going to block
	if (!SocketReadable(&connectSocket)) { return; }

	iResult = recv(connectSocket, recvbuf, recvbuflen, 0);
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
	Network::Send(connectSocket, sendBuf);
}

void Client::Disconnect()
{
	// the client can still use the connectSocket for receiving data
	int iResult = shutdown(connectSocket, SD_SEND);
	closesocket(connectSocket);
	if (iResult == SOCKET_ERROR) {
		std::cout << "shutdown failed: " << WSAGetLastError() << "\n";
		WSACleanup();
		return;
	}
}

void Client::Close()
{
	// cleanup
	closesocket(connectSocket);
	WSACleanup();
}
