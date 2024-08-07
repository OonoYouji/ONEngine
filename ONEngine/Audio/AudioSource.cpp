#include "AudioSource.h"

#include <d3d12.h>
#include <xaudio2.h>

#include <cassert>

#include <AudioManager.h>


void AudioSource::PlayAudio() {
	HRESULT result = S_FALSE;

	elem.pSourceVoice = clip_->CreateSourceVoice();

	///- 再生する波形データの設定
	XAUDIO2_BUFFER buffer{};
	buffer.pAudioData = clip_->pBuffer;
	buffer.AudioBytes = clip_->bufferSize;
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	///- 波形データの再生
	elem.pSourceVoice->SubmitSourceBuffer(&buffer);
	elem.pSourceVoice->Start();
}

void AudioSource::StopAudio() {
	elem.pSourceVoice->Stop();
	elem.pSourceVoice->FlushSourceBuffers();
}

void AudioSource::PauseAudio() {
	elem.pSourceVoice->Stop();
}

void AudioSource::SetAudioClip(const std::string& filePath) {
	clip_ = AudioManager::GetInstance()->GetAudioClip(filePath);
}
