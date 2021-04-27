#pragma once
#include "networking/Client.hpp"
#include <glm/glm.hpp>
#include <unordered_map>

struct Player {
	uint64_t playerId;
	glm::vec2 position;
};

class SimpleClient : public Client {
public:
	SimpleClient(const std::string& ip, const char* port)
		: Client{ip, port} {}

	void handleServerMessage(const Message& message) override;
	void sendPositionUpdate(float x, float y);

	uint64_t getPlayerId() { return playerId; }
	const std::unordered_map<uint64_t, Player>& getPlayers() { return players; }
private:
	uint64_t playerId = 0;
	std::unordered_map<uint64_t, Player> players;
};