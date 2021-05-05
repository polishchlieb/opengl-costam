#include "AudioContext.hpp"
#include <stdexcept>

AudioContext::AudioContext(const char* deviceName) {
	init(deviceName);
}

AudioContext::AudioContext() {
	const ALCchar* defaultDeviceString = alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);
	init(defaultDeviceString);
}

AudioContext::~AudioContext() {
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(context);
	alcCloseDevice(device);
}

void AudioContext::init(const char* deviceName) {
	device = alcOpenDevice(deviceName);
	if (!device)
		throw std::runtime_error("Couldn't open the device");

	context = alcCreateContext(device, nullptr);
	if (!alcMakeContextCurrent(context))
		throw std::runtime_error("Couldn't make the context current");
}
