#include "EventDispatcher.hpp"

std::vector<std::function<void(std::shared_ptr<Event>)>> EventDispatcher::listenerFunctions;