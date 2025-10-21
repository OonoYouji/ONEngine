#pragma once

/// std
#include <list>
#include <string>

/// audio
#include <xaudio2.h>

/// engine
#include "../Interface/ECSISystem.h"
#include "Engine/Core/DirectX12/ComPtr/ComPtr.h"
	
/// ////////////////////////////////////////////////////////////
/// 音の再生を行うクラス
/// ////////////////////////////////////////////////////////////
class AudioPlaybackSystem : public ECSISystem {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	AudioPlaybackSystem(class AssetCollection* _resourceCollection);
	~AudioPlaybackSystem();

	void OutsideOfRuntimeUpdate(ECSGroup* _ecs) override;
	void RuntimeUpdate(ECSGroup* _ecs) override;

private:
	/// ==================================================
	/// private : methods
	/// ==================================================


	/// 設定
	void SetAudioClip(class AudioSource* _audioSource);

	/// 再生
	void PlayAudio(class AudioSource* _audioSource);
	void PlayOneShot(class AudioClip* _audioClip, float _volume, float _pitch, const std::string& _path);

	/// 状態の取得
	int GetAudioState(class AudioSource* _audioSource);

private:
	/// ===================================================
	/// private : methods
	/// ===================================================
	
	/// other classes
	class AssetCollection* pGrc_ = nullptr;

	/// xAudio
	ComPtr<IXAudio2> xAudio2_ = nullptr;
	IXAudio2MasteringVoice* masterVoice_ = nullptr;

	/// one shot audios
	std::list<IXAudio2SourceVoice*> oneShotAudios_;

	float masterVolume_;

};

