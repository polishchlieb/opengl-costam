#pragma once
#include "networking/Client.hpp"
#include <glm/glm.hpp>

enum SocketEventType {
	PLAYER_MOVE,
	HELLO,
	PLAYER_JOIN
};

struct Player {
	uint64_t playerId;
	glm::vec2 position;
};

class SimpleClient : public Client {
public:
	SimpleClient(const std::string& ip, const char* port)
		: Client{ip, port} {}

	void handleConnection() override {
		std::cout << "hell world from server" << std::endl;
	}
	
	void handleServerMessage(const Message& message) {
		std::cout << "message from server" << std::endl;

		SocketEventType type;
		message.read(type);

		switch (type) {
			case HELLO:
				message.read(playerId);

				size_t playerCount;
				message.read(playerCount);
				players.reserve(playerCount);
				
				for (size_t i = 0; i < playerCount; ++i) {
					uint64_t playerId;
					message.read(playerId);

					float x, y;
					message.read(x);
					message.read(y);

					players.insert({playerId, {playerId, glm::vec2{x, y}}});
				}

				break;
			case PLAYER_MOVE:
				uint64_t playerId;
				message.read(playerId);

				float x, y;
				message.read(x);
				message.read(y);

				players[playerId].position = {x, y};

				break;
			default:
				std::cout << "unknown type" << std::endl;
		}
	}

	void handleDisconnection() override {
		std::cout << "disconnected" << std::endl;
	}

	void handleError(int id) override {
		std::cout << "error id " << id << std::endl;
	}

	uint32_t playerId = 0;
	std::unordered_map<uint64_t, Player> players;
};