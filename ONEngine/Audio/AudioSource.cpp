#include "AudioSource.h"

#include <d3d12.h>
#include <xaudio2.h>

#include <cassert>

#include <AudioManager.h>


AudioSource::AudioSource() {
	AudioManager::GetInstance()->AddAudioSource(this);
}

void AudioSource::Update() {
	for(auto& source : sources_) {
		source.pSourceVoice->SetVolume(volume);
		source.pSourceVoice->SetFrequencyRatio(pitch);
	}

	if(!isLoop) {
		for(auto& source : sources_) {
			source.pSourceVoice->ExitLoop();
		}
	}

	XAUDIO2_VOICE_STATE state{};
	for(auto it = sources_.begin(); it != sources_.end();) {
		auto& source = *it;
		if(source.pSourceVoice) {
			source.pSourceVoice->GetState(&state);
			if(state.BuffersQueued == 0) {
				source.pSourceVoice->DestroyVoice();
				it = sources_.erase(it);
			} else {
				++it;
			}
		} else {
			it = sources_.erase(it);
		}
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
	if(isLoop) {
		buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

	///- 波形データの再生
	elem.pSourceVoice->SubmitSourceBuffer(&buffer);
	elem.pSourceVoice->SetVolume(volume);
	elem.pSourceVoice->SetFrequencyRatio(pitch);
	elem.pSourceVoice->Start();

	sources_.push_back(std::move(elem));

}

void AudioSource::StopAudio() {
	if(sources_.empty()) { return; }
	Element* elem = &sources_.back();
	elem->pSourceVoice->Stop();
	elem->pSourceVoice->FlushSourceBuffers();
	elem->pSourceVoice->DestroyVoice();
	sources_.pop_back();
}

void AudioSource::StopAudioAll() {
	if(sources_.empty()) { return; }
	for(auto& source : sources_) {
		source.pSourceVoice->Stop();
		source.pSourceVoice->FlushSourceBuffers();
		source.pSourceVoice->DestroyVoice();
	}
	sources_.clear();
}


void AudioSource::SetAudioClip(const std::string& filePath) {
	clip_ = AudioManager::GetInstance()->GetAudioClip(filePath);
}

