#include "Client.hpp"

Client::Client(const std::string& ip, const char* port) {
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
		throw std::runtime_error("couldn't initialize the connection");

	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	result = getaddrinfo(ip.c_str(), port, &hints, &addressInfo);
	if (result != 0)
		throw std::runtime_error("failed to resolve address or port");
}

void Client::connect() {
	addrinfo* ptr;
	for (ptr = addressInfo; ptr != NULL; ptr = ptr->ai_next) {
		handle = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (handle == INVALID_SOCKET)
			throw std::runtime_error("failed to connect");

		int result = ::connect(handle, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen));
		if (result == SOCKET_ERROR)
			throw std::runtime_error("failed to connect");

		break;
	}

	freeaddrinfo(addressInfo);

	if (handle == INVALID_SOCKET)
		throw std::runtime_error("unable to connect to server");
	handleConnection();

	static const size_t bufferSize = 1024;
	char buffer[bufferSize];
	while (true) {
		int result = recv(handle, buffer, bufferSize, 0);
		if (result > 0) {
			handleServerMessage(Message{buffer});
		} else if (result == -1) {
			break;
		} else {
			handleError(WSAGetLastError());
		}
	}

	handleDisconnection();
}

void Client::disconnect() {
	/* if (shutdown(handle, SD_SEND) == SOCKET_ERROR)
		throw std::runtime_error("failed to disconnect"); */
	shutdown(handle, SD_SEND);

	closesocket(handle);
	WSACleanup();
}

void Client::send(const Message& message) const {
	int result = ::send(handle, message.getBuffer(), static_cast<int>(message.size), 0);
	if (result == SOCKET_ERROR)
		throw std::runtime_error("failed to send the message");
}
