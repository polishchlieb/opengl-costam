#pragma once
#include <AL/al.h>
#include <AL/alc.h>
#include <string>
#include <array>
#include <AudioFile.h>

class AudioSource {
public:
	AudioSource() = default;
	~AudioSource();

	void load(std::string fileName);

	void play();
	void pause();
	bool isPlaying();

	void setPosition(std::array<float, 3> value);
	void setVelocity(std::array<float, 3> value);
	void setPitch(float value);
	void setGain(float value);
	void setLooping(bool value);
private:
	ALuint source, buffer;
};