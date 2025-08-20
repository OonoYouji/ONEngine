#include "AudioSource.h"

AudioSource::AudioSource() {
	volume_ = 1.0f;
	pitch_ = 1.0f;
}

AudioSource::~AudioSource() {}

void AudioSource::Play() {
	isPlayingRequest_ = true;
}

void AudioSource::SetVolume(float _volume) {
	volume_ = _volume;
}

void AudioSource::SetPitch(float _pitch) {
	pitch_ = _pitch;
}

void AudioSource::SetAudioPath(const std::string& _path) {
	path_ = _path;
}

float AudioSource::GetVolume() const {
	return volume_;
}

float AudioSource::GetPitch() const {
	return pitch_;
}

const std::string& AudioSource::GetAudioPath() const {
	return path_;
}
