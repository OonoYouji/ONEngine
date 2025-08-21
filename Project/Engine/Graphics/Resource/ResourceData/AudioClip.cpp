#include "AudioClip.h"



void SoundDataUnload(AudioStructs::SoundData* _soundData) {
	/// 音データの解放をする
	_soundData->buffer.clear();
	_soundData->wfex = {};
}
