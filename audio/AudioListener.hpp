#pragma once
#include <AL/al.h>
#include <AL/alc.h>
#include <array>

class AudioListener {
public:
	static void setPosition(std::array<float, 3> value) {
		alListener3f(AL_POSITION, value[0], value[1], value[2]);
	}

	static void setVelocity(std::array<float, 3> value) {
		alListener3f(AL_VELOCITY, value[0], value[1], value[2]);
	}

	static void setOrientation(std::array<float, 6> value) {
		alListenerfv(AL_ORIENTATION, value.data());
	}
};