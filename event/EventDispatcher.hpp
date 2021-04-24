#pragma once
#include <vector>
#include <memory>
#include <functional>

#include "Event.hpp"

class EventDispatcher {
public:
	static void dispatch(std::shared_ptr<Event> event) {
		for (const auto& listener : listenerFunctions) {
			listener(event);
		}
	}

	static void addListener(std::function<void(std::shared_ptr<Event>)> fn) {
		listenerFunctions.push_back(std::move(fn));
	}

private:
	static std::vector<std::function<void(std::shared_ptr<Event>)>> listenerFunctions;
};