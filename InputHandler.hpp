#pragma once

class InputHandler {
public:
	InputHandler();
	void pollEvents();

	virtual void handleMouseInput();
	virtual void handleKeyboardInput();
protected:

};