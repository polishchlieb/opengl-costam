#pragma once
#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#include <string>
#include <stdexcept>

#include "Message.hpp"

class Client {
public:
	Client(const std::string& ip, const char* port);
	~Client() { disconnect(); }

	void connect();
	void disconnect();
	void send(const Message& message) const;

	virtual void handleConnection() {}
	virtual void handleServerMessage(const Message& message) {}
	virtual void handleDisconnection() {}
	virtual void handleError(int id) {}
private:
	SOCKET handle = INVALID_SOCKET;
	addrinfo* addressInfo;
};