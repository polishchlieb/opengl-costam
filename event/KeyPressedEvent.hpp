#pragma once
#include "Event.hpp"

class KeyPressedEvent : public Event {
public:
	KeyPressedEvent(int key) : key(key) {}
	int key;
};