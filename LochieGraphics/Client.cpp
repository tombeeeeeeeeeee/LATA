#include "Client.h"

int Client::Start()
{
	std::cout << "Starting Client...\n";

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
	hints.ai_family = AF_INET; // IPv4 (AF_INET)		// IPv6 (AF_INET6)
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;


	// Resolve the server address and port
	iResult = getaddrinfo(address.c_str(), port.c_str(), &hints, &result);
	if (iResult != 0) {
		std::cout << "getaddrinfo failed: " << iResult << "\n";
		WSACleanup();
		return -1;
	}
	else {
		std::cout << "Address and port resolved!\n";
	}



	// Attempt to connect to the first address returned by
	// the call to getaddrinfo
	ptr = result;

	// Create a SOCKET for connecting to server
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	if (ConnectSocket == INVALID_SOCKET) {
		std::cout << "Error at socket(): " << WSAGetLastError() << "\n";
		freeaddrinfo(result);
		WSACleanup();
		return -1;
	}
	else {
		std::cout << "Socket made!\n";
	}



	//WSAPoll()


	//WSAPOLLFD fdArray = { 0 };
	//fdArray.events = POLLWRNORM;
	//fdArray.fd = ConnectSocket;

	//int check = WSAPoll(&fdArray, 1, 0);

	//std::cout << "Socket check is: " << check << "\n";

	//while (check == 0)
	//{
	//	check = WSAPoll(&fdArray, 1, 0);

	//}
	//std::cout << "Socket check is: " << check << "\n";





	// Connect to server.
	iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
		std::cout << "Unable to connect to server: " << WSAGetLastError() << "\n";
		WSACleanup();
		return -1;

	}
	else {
		std::cout << "Connected to server!\n";
	}


	// Set socket to non blocking mode
	iResult = ioctlsocket(ConnectSocket, FIONBIO, &uNonBlockingMode);
	if (iResult == SOCKET_ERROR) {
		std::cout << "Couldn't set socket mode: " << WSAGetLastError() << "\n";
		freeaddrinfo(result);
		closesocket(ConnectSocket);
		WSACleanup();
		return -1;
	}



	// Should really try the next address returned by getaddrinfo
	// if the connect call failed
	// But for this simple example we just free the resources
	// returned by getaddrinfo and print an error message
	freeaddrinfo(result);
	//if (ConnectSocket == INVALID_SOCKET) {
	//	std::cout << "Unable to connect to server!\n";
	//	WSACleanup();
	//	return -1;
	//}
	//else {
	//	std::cout << "Connected to server!\n";
	//}

	return 0;
}

int Client::Run() // check if gonna block
{
	int iResult;

	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	WSAPOLLFD fdArray = { 0 };
	fdArray.events = POLLRDNORM;
	fdArray.fd = ConnectSocket;

	int check = WSAPoll(&fdArray, 1, 0);

	if (check == 0) {
		return 2;
	}



	iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
	if (iResult > 0) {
		std::cout << "Bytes received: " << iResult << "\n";

		// TODO: Callback here
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

	return 0;
}

int Client::Send(const char* sendBuf)
{
	// Send an initial buffer (Plus one for terminating char \0)
	int iResult = send(ConnectSocket, sendBuf, (int)strlen(sendBuf) + 1, 0);
	if (iResult == SOCKET_ERROR) {
		std::cout << "send failed: " << WSAGetLastError() << "\n";
		closesocket(ConnectSocket);
		WSACleanup();
		return -1;
	}
	std::cout << "Bytes Sent: " << iResult << "\n";

	return 0;
}

int Client::Disconnect()
{
	// the client can still use the ConnectSocket for receiving data
	int iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		std::cout << "shutdown failed: " << WSAGetLastError() << "\n";
		closesocket(ConnectSocket);
		WSACleanup();
		return -1;
	}
	return 0;
}

int Client::Close()
{
	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();
	return 0;
}
