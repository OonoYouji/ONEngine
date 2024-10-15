#include "AudioClip.h"

#include <cassert>
#include "AudioManager.h"

void AudioClip::Release() {
	delete[] pBuffer;
	pBuffer = 0;
	bufferSize = 0;
	wfex = {};
}

IXAudio2SourceVoice* AudioClip::CreateSourceVoice() {
	IXAudio2SourceVoice* sourceVoice = nullptr;
	HRESULT result = AudioManager::GetInstance()->GetxAudio2()->CreateSourceVoice(&sourceVoice, &wfex);
	assert(SUCCEEDED(result));
	return sourceVoice;
}
