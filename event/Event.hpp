#pragma once
#include "EventType.hpp"

class Event {
public:
	EventType type;
	bool handled = false;
};