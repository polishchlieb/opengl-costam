#pragma once
#include "../Event.hpp"

class MouseMoveEvent : public Event {
public:
	float dx, dy;
};