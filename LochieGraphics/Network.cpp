#include "Network.h"

ULONG Network::uNonBlockingMode = 1;

bool Network::CommonSetup(std::string ip, std::string port, addrinfo** info, SOCKET* soc)
{
	if (!Network::InitWinsock()) { return false; }

	addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET; // IPv4 (AF_INET)		// IPv6 (AF_INET6)
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE; // Only needed for the server, but ingored for client (only gets used for a bind call)

	if (!Network::GetAddressInfo(ip, port, &hints, info)) { return false; }

	if (!Network::CreateSocket(soc, info)) { return false; }

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
