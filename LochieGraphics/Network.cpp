#include "Network.h"

ULONG Network::uNonBlockingMode = 1;

bool Network::CommonSetup(addrinfo** info, SOCKET* soc)
{
	// Initialise WinSock
	if (!InitWinsock()) { return false; }

	addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	// AF_INET is IPv4, AF_INET6 is IPv6
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	// Only needed for the server, but ignored for client (only gets used for a bind call)
	hints.ai_flags = AI_PASSIVE;

	if (!GetAddressInfo(address, port, &hints, info)) { return false; }

	if (!CreateSocket(soc, info)) { return false; }

	return true;
}

bool Network::InitWinsock()
{
	WSADATA wsaData;
	int error = WSAStartup(MAKEWORD(2, 2), &wsaData); // Makeword specifies the version
	if (error != 0) {
		std::cout << "WSAStartup failed: " << error << "\n";
		return false;
	}
    return true;
}
// TODO: Should this not be checking for writability?
bool Network::Send(SOCKET soc, const char* sendBuffer)
{
	// (Plus one for terminating char \0)
	int sent = send(soc, sendBuffer, (int)strlen(sendBuffer) + 1, 0);
	if (sent == SOCKET_ERROR) {
		std::cout << "Send failed: " << WSAGetLastError() << "\n";
		closesocket(soc); // TODO: Does it really need to close the whole socket here?
		WSACleanup();
		return false;
	}
	std::cout << "Bytes Sent: " << sent << "\n";
	return true;
}

bool Network::GetAddressInfo(std::string ip, std::string port, addrinfo* hints, addrinfo** info)
{
	int error = getaddrinfo(ip.c_str(), port.c_str(), hints, info);
	if (error != 0) {
		std::cout << "getaddrinfo failed: " << error << "\n";
		WSACleanup();
		return false;
	}
	return true;
}

bool Network::CreateSocket(SOCKET* soc, addrinfo** info)
{
	*soc = socket((*info)->ai_family, (*info)->ai_socktype, (*info)->ai_protocol);
	if (*soc == INVALID_SOCKET) {
		std::cout << "Error creating socket: " << WSAGetLastError() << "\n";
		freeaddrinfo(*info);
		WSACleanup();
		return false;
	}
	return true;
}

bool Network::SocketReadable(SOCKET* soc)
{
	WSAPOLLFD fdArray = { 0 };
	fdArray.events = POLLRDNORM;
	fdArray.fd = *soc;
	int check = WSAPoll(&fdArray, 1, 0);
	if (check == SOCKET_ERROR) {
		std::cout << "Error while checking readability of socket: " << WSAGetLastError() << "\n";
		return false;
	}
	return check; // Check will be 1 or greater when readable
}

bool Network::SocketWritable(SOCKET* soc)
{
	// TODO: Write function
	std::cout << "ERROR CHECKING WRITABLITIY, I haven't made the function yet\n";
	return false;
}

bool Network::SetSocketNonBlocking(SOCKET* soc, addrinfo** info)
{
	int error = ioctlsocket(*soc, FIONBIO, &uNonBlockingMode);
	if (error == SOCKET_ERROR) {
		std::cout << "Couldn't set socket mode: " << WSAGetLastError() << "\n";
		freeaddrinfo(*info);
		closesocket(*soc);
		WSACleanup();
		return false;
	}
	return true;
}
