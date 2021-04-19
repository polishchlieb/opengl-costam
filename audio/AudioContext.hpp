#pragma once
#include <AL/alc.h>

class AudioContext {
public:
	AudioContext(const char* deviceName);
	AudioContext();
	~AudioContext();

private:
	ALCdevice* device;
	ALCcontext* context;
	
	void init(const char* deviceName);
};