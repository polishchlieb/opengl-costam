#include "SimpleClient.hpp"

enum SocketEventType {
	PLAYER_MOVE,
	HELLO,
	PLAYER_JOIN,
	PLAYER_DISCONNECT
};

void SimpleClient::handleServerMessage(const Message& message) {
	SocketEventType type;
	message.read(type);

	switch (type) {
		case HELLO: {
			message.read(playerId);

			uint64_t playerCount;
			message.read(playerCount);
			if (playerCount > 0) {
				std::cout << playerCount << std::endl;
				players.reserve(playerCount);

				for (uint64_t i = 0; i < playerCount; ++i) {
					uint64_t playerId;
					message.read(playerId);

					float x, y;
					message.read(x);
					message.read(y);

					players.insert({ playerId, {playerId, glm::vec2{x, y}} });
				}
			}

			break;
		}
		case PLAYER_MOVE: {
			uint64_t playerId;
			message.read(playerId);

			float x, y;
			message.read(x);
			message.read(y);

			players[playerId].position = { x, y };

			break;
		}
		case PLAYER_JOIN: {
			uint64_t playerId;
			message.read(playerId);

			players[playerId] = {playerId, {0.f, 0.f}};
			break;
		}
		case PLAYER_DISCONNECT: {
			uint64_t playerId;
			message.read(playerId);
			players.erase(playerId);
			break;
		}
		default:
			std::cout << "unknown type" << std::endl;
	}
}

void SimpleClient::sendPositionUpdate(float x, float y) {
	Message m;
	m.push<SocketEventType>(PLAYER_MOVE);
	m.push<float>(x);
	m.push<float>(y);
	send(m);
}
