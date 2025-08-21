#include "AudioClip.h"

AudioClip::~AudioClip() {
	SoundDataUnload(&soundData_);
}

void SoundDataUnload(AudioStructs::SoundData* _soundData) {
	/// 音データの解放をする
	_soundData->buffer.clear();
	_soundData->wfex = {};
}

