#include "AudioSource.h"

#include <d3d12.h>
#include <xaudio2.h>

#include <cassert>

#include <AudioManager.h>


void AudioSource::Update() {
	for(auto& source : sources_) {
		source.pSourceVoice->SetVolume(volume_);
		source.pSourceVoice->SetFrequencyRatio(pitch_);
	}
}

void AudioSource::PlayAudio() {
	HRESULT result = S_FALSE;

	Element elem{};
	elem.pSourceVoice = clip_->CreateSourceVoice();

	///- 再生する波形データの設定
	XAUDIO2_BUFFER buffer{};
	buffer.pAudioData = clip_->pBuffer;
	buffer.AudioBytes = clip_->bufferSize;
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	///- 波形データの再生
	elem.pSourceVoice->SubmitSourceBuffer(&buffer);
	elem.pSourceVoice->SetVolume(volume_);
	elem.pSourceVoice->Start();

	sources_.push_back(std::move(elem));

}

void AudioSource::StopAudio() {
	Element& elem = sources_.back();
	elem.pSourceVoice->Stop();
	elem.pSourceVoice->FlushSourceBuffers();
	elem.pSourceVoice->DestroyVoice();
}

void AudioSource::StopAudioALL() {
	for(auto& source : sources_) {
		source.pSourceVoice->Stop();
		source.pSourceVoice->FlushSourceBuffers();
		source.pSourceVoice->DestroyVoice();
	}
}


void AudioSource::SetAudioClip(const std::string& filePath) {
	clip_ = AudioManager::GetInstance()->GetAudioClip(filePath);
}
