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


/// 

void COMP_DEBUG::AudioSourceDebug(AudioSource* _as) {
	if(!_as) {
		return;
	}




}



/// json serialize
void from_json(const nlohmann::json& _j, AudioSource& _a) {
	_a.enable = _j.value("enable", 1);
	_a.SetVolume(_j.value("volume", 1.0f));
	_a.SetPitch(_j.value("pitch", 1.0f));
	_a.SetAudioPath(_j.value("path", std::string("")));
}

void to_json(nlohmann::json& _j, const AudioSource& _a) {
	_j = nlohmann::json{
		{ "type", "AudioSource" },
		{ "enable", _a.enable },
		{ "volume", _a.GetVolume() },
		{ "pitch", _a.GetPitch() },
		{ "path", _a.GetAudioPath() }
	};
}
