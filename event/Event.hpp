#pragma once
#include "EventType.hpp"

class Event {
public:
	EventType type = NONE;
	bool handled = false;
};