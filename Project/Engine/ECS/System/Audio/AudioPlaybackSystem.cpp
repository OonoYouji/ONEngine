#include "AudioPlaybackSystem.h"

/// engine
#include "Engine/ECS/EntityComponentSystem/ECSGroup.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Audio/AudioSource.h"
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Graphics/Resource/ResourceData/AudioClip.h"

using namespace AudioStructs;

AudioPlaybackSystem::AudioPlaybackSystem(AssetCollection* _resourceCollection)
	: pGrc_(_resourceCollection) {

	HRESULT hr = S_FALSE;

	/// xAudioインスタンスの生成
	hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	Assert(SUCCEEDED(hr));

	/// MasterVoiceの生成
	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
	Assert(SUCCEEDED(hr));
}

AudioPlaybackSystem::~AudioPlaybackSystem() {}


void AudioPlaybackSystem::OutsideOfRuntimeUpdate(ECSGroup* _ecs) {}

void AudioPlaybackSystem::RuntimeUpdate(ECSGroup* _ecs) {
	///
	ComponentArray<AudioSource>* asArray = _ecs->GetComponentArray<AudioSource>();
	if (!asArray || asArray->GetUsedComponents().empty()) {
		return;
	}

	for (auto& as : asArray->GetUsedComponents()) {
		/// 有効なAudioSourceのみ処理する
		if (!as || !as->enable) {
			continue;
		}

		/// 音のクリップを設定
		SetAudioClip(as);

		/// 音の再生リクエストチェック
		if (as->isPlayingRequest_) {
			/// 再生状態ではなければ再生する
			if (as->state_ != AudioState_Playing) {
				PlayAudio(as);
			}
		}

		/// 音の状態を取得
		int state = GetAudioState(as);
		if (state != as->state_) {
			/// 状態が変わった場合は更新する
			as->state_ = state;
		}


		/// OneShotAudioの再生リクエストチェック
		for(auto& req : as->oneShotAudioRequests_) {
			/// ワンショット再生
			AudioClip* clip = pGrc_->GetAudioClip(req.path);
			PlayOneShot(clip, req.volume, req.pitch, req.path);
		}

		/// ワンショット再生が終わった音声ソースを削除
		as->oneShotAudioRequests_.clear();

	}

}

void AudioPlaybackSystem::SetAudioClip(AudioSource* _audioSource) {
	AudioClip* clip = pGrc_->GetAudioClip(_audioSource->path_);
	if (clip) {
		_audioSource->pAudioClip_ = clip;
	}
}

void AudioPlaybackSystem::PlayAudio(AudioSource* _audioSource) {
	/// stateをPlayingに変更
	_audioSource->state_ = AudioState_Playing;
	_audioSource->isPlayingRequest_ = false;

	IXAudio2SourceVoice* sourceVoice = nullptr;
	sourceVoice = _audioSource->pAudioClip_->CreateSourceVoice(xAudio2_.Get());

	/// 再生する波形データの設定
	const SoundData& soundData = _audioSource->pAudioClip_->GetSoundData();
	XAUDIO2_BUFFER buffer{};
	buffer.pAudioData = soundData.buffer.data();
	buffer.AudioBytes = static_cast<UINT32>(soundData.buffer.size());
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	//if (isLoop) {
	//	buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	//}

	/// 波形データの再生
	sourceVoice->SubmitSourceBuffer(&buffer);
	sourceVoice->SetVolume(_audioSource->volume_);
	sourceVoice->SetFrequencyRatio(_audioSource->pitch_);
	sourceVoice->Start();

	/// 音声ソースをAudioSourceに追加
	_audioSource->sourceVoices_.push_back(sourceVoice);
}

void AudioPlaybackSystem::PlayOneShot(AudioClip* _audioClip, float _volume, float _pitch, const std::string& _path) {
	IXAudio2SourceVoice* sourceVoice = nullptr;
	sourceVoice = _audioClip->CreateSourceVoice(xAudio2_.Get());

	/// 再生する波形データの設定
	const SoundData& soundData = _audioClip->GetSoundData();
	XAUDIO2_BUFFER buffer{};
	buffer.pAudioData = soundData.buffer.data();
	buffer.AudioBytes = static_cast<UINT32>(soundData.buffer.size());
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	/// 波形データの再生
	sourceVoice->SubmitSourceBuffer(&buffer);
	sourceVoice->SetVolume(_volume);
	sourceVoice->SetFrequencyRatio(_pitch);
	sourceVoice->Start();

	/// 音声ソースをAudioSourceに追加
	oneShotAudios_.push_back(sourceVoice);
}

int AudioPlaybackSystem::GetAudioState(AudioSource* _audioSource) {
	AudioClip* clip = _audioSource->pAudioClip_;
	if (!clip) {
		return AudioState_Stopped; // クリップが設定されていない場合は停止状態
	}


	std::list<IXAudio2SourceVoice*>& sourceVoices = _audioSource->sourceVoices_;
	for (auto itr = sourceVoices.begin(); itr != sourceVoices.end();) {
		IXAudio2SourceVoice* sourceVoice = *itr;
		if (!sourceVoice) {
			itr = sourceVoices.erase(itr);
			continue;
		}

		XAUDIO2_VOICE_STATE state;
		sourceVoice->GetState(&state);
		if (state.BuffersQueued == 0) {
			itr = sourceVoices.erase(itr);
			continue;
		}

		itr++;
	}

	if(sourceVoices.size() > 0) {
		/// 再生中の音声ソースがある場合は再生中
		return AudioState_Playing;
	}

	return AudioState_Stopped; // 停止中
}
