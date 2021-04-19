#include "AudioSource.hpp"

AudioSource::~AudioSource() {
	alDeleteSources(1, &source);
	alDeleteBuffers(1, &buffer);
}

void AudioSource::load(std::string fileName) {
	AudioFile<float> soundFile;
	if (!soundFile.load(std::move(fileName)))
		throw std::runtime_error("Couldn't load amogus.wav");

	std::vector<uint8_t> monoPCMDataBytes;
	soundFile.writePCMToBuffer(monoPCMDataBytes);

	int bitDepth = soundFile.getBitDepth();
	int alFormat;
	if (bitDepth == 16)
		alFormat = soundFile.isStereo() ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
	else if (bitDepth == 8)
		alFormat = soundFile.isStereo() ? AL_FORMAT_STEREO8 : AL_FORMAT_MONO8;
	else
		alFormat = -1;

	alGenBuffers(1, &buffer);
	alBufferData(
		buffer,
		alFormat,
		monoPCMDataBytes.data(),
		static_cast<ALsizei>(monoPCMDataBytes.size()),
		soundFile.getSampleRate()
	);

	alGenSources(1, &source);

	setPosition({1.f, 0.f, 0.f});
	setVelocity({0.f, 0.f, 0.f});
	setPitch(1.f);
	setGain(1.f);
	setLooping(false);

	alSourcei(source, AL_BUFFER, buffer);
}

void AudioSource::play() {
	alSourcePlay(source);
}

void AudioSource::pause() {
	alSourcePause(source);
}

bool AudioSource::isPlaying() {
	ALint state;
	alGetSourcei(source, AL_SOURCE_STATE, &state);
	return state == AL_PLAYING;
}

void AudioSource::setPosition(std::array<float, 3> value) {
	alSource3f(source, AL_POSITION, value[0], value[1], value[2]);
}

void AudioSource::setVelocity(std::array<float, 3> value) {
	alSource3f(source, AL_VELOCITY, value[0], value[1], value[2]);
}

void AudioSource::setPitch(float value) {
	alSourcef(source, AL_PITCH, value);
}

void AudioSource::setGain(float value) {
	alSourcef(source, AL_GAIN, value);
}

void AudioSource::setLooping(bool value) {
	alSourcei(source, AL_LOOPING, value ? AL_TRUE : AL_FALSE);
}
