#pragma once

/// std
#include <list>
#include <string>

/// audio
#include <xaudio2.h>

/// engine
#include "../Interface/ECSISystem.h"
#include "Engine/Core/DirectX12/ComPtr/ComPtr.h"

namespace ONEngine {
class ECSGroup;
class AudioSource;
}

namespace ONEngine::Asset {
class AssetCollection;
class AudioClip;
}


/// ////////////////////////////////////////////////////////////
/// 音の再生を行うクラス
/// ////////////////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class AudioPlaybackSystem
 * @brief XAudio2を用いたオーディオ再生・音声処理（フェード、ワンショット再生、ボリューム制御等）を司るシステムクラス
 */
class AudioPlaybackSystem : public ECSISystem {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 * @param _assetCollection アセット読み込み用のコレクションクラスへのポインタ
	 */
	AudioPlaybackSystem(Asset::AssetCollection* _assetCollection);

	/**
	 * @brief デストラクタ
	 */
	~AudioPlaybackSystem();

	/**
	 * @brief エディタ非実行時のオーディオ更新処理（主に停止状態のクリーンアップなど）を行います。
	 */
	void OutsideOfRuntimeUpdate(ECSGroup* _ecs) override;

	/**
	 * @brief 毎フレームのオーディオ再生状態の更新処理（BGM/SEの再生開始・停止、ワンショットSEのクリーンアップ）を実行します。
	 */
	void RuntimeUpdate(ECSGroup* _ecs) override;

private:
	/// ==================================================
	/// private : methods
	/// ==================================================


	/**
	 * @brief AudioSourceコンポーネントに対し、アセット情報に基づきXAudio2のソースボイス・バッファ構築を設定します。
	 */
	void SetAudioClip(AudioSource* _audioSource);

	/**
	 * @brief 指定したAudioSourceコンポーネントの音声を再生開始します。
	 */
	void PlayAudio(AudioSource* _audioSource);

	/**
	 * @brief SEのワンショット（一度限りの重ね合わせ再生）再生を行います。
	 */
	void PlayOneShot(Asset::AudioClip* _audioClip, float _volume, float _pitch, const std::string& _path);

	/**
	 * @brief AudioSourceコンポーネントが現在再生中か、一時停止中かなどの状態（XAudio2のステート）を取得します。
	 */
	int GetAudioState(AudioSource* _audioSource);

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	/// other classes
	Asset::AssetCollection* pAssetCollection_ = nullptr;

	/// xAudio
	ComPtr<IXAudio2> xAudio2_ = nullptr;
	IXAudio2MasteringVoice* masterVoice_ = nullptr;

	/// one shot audios
	std::list<IXAudio2SourceVoice*> oneShotAudios_;

	float masterVolume_;

};


} /// ONEngine
