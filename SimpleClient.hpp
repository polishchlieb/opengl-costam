#pragma once
#include "networking/Client.hpp"

class SimpleClient : public Client {
public:
	SimpleClient(const std::string& ip, const char* port)
		: Client{ip, port} {}

	void handleConnection() override {
		std::cout << "hell world from server" << std::endl;
	}
	
	void handleServerMessage(const Message& message) {
		std::cout << "message from server" << std::endl;
	}

	void handleDisconnection() override {
		std::cout << "disconnected" << std::endl;
	}

	void handleError(int id) override {
		std::cout << "error id " << id << std::endl;
	}
};