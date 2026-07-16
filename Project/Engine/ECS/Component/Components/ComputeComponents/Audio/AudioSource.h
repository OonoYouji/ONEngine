#pragma once

/// std
#include <string>
#include <list>
#include <vector>

/// audio
#include <xaudio2.h>

/// external
#include <nlohmann/json.hpp>
#include <mono/jit/jit.h>

/// engine
#include "../../Interface/IComponent.h"

namespace ONEngine::Asset {
class AudioClip;
}


namespace ONEngine {

/// @brief 音の再生状態
enum class AudioState {
	Stopped,
	Playing,
	Paused,
};

/// @brief 一度キリの再生に使う構造体
struct OneShotAudio {
	std::string path;
	float volume;
	float pitch;
};

/// ////////////////////////////////////////////////////////////
/// Audio Source
/// ////////////////////////////////////////////////////////////
/**
 * @class AudioSource
 * @brief ゲームオーディオアセット（AudioClip）のロード、ボリューム・ピッチ制御、ループ再生、およびワンショット（OneShot）効果音再生を管理するオーディオソースコンポーネントクラス
 */
class AudioSource : public IComponent {
	friend class AudioPlaybackSystem;
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	AudioSource();

	/**
	 * @brief デストラクタ
	 */
	~AudioSource();

	/**
	 * @brief 音声ファイルの再生を開始（または一時停止から復帰）します。
	 */
	void Play();

	/**
	 * @brief 音声ファイルの再生を停止します。
	 */
	void Stop();

	/**
	 * @brief 指定されたパスの音声（効果音等）を指定音量・ピッチで一度限り重複再生（ワンショット再生）します。
	 * @param _volume 再生音量
	 * @param _pitch 再生ピッチ
	 * @param _path オーディオファイルのアセット相対パス
	 */
	void PlayOneShot(float _volume, float _pitch, const std::string& _path);

	/**
	 * @brief 再生対象となるXAudio2のソースボイスを追加登録します。
	 * @param _sourceVoice XAudio2ソースボイス
	 */
	void AddSourceVoice(IXAudio2SourceVoice* _sourceVoice);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	Asset::AudioClip* pAudioClip_;
	std::string path_;
	float volume_;
	float pitch_;

	int state_;
	bool isPlayingRequest_;
	bool isStopRequest_ = false;
	bool isLoop_ = false;

	/// 再生中の音声ソースリスト
	std::list<IXAudio2SourceVoice*> sourceVoices_;

	/// ワンショット再生リクエストリスト
	std::vector<OneShotAudio> oneShotAudioRequests_;

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/// ----- setter ----- ///

	/**
	 * @brief 音量を設定します。
	 */
	void SetVolume(float _volume);

	/**
	 * @brief 再生ピッチ（周波数比）を設定します。
	 */
	void SetPitch(float _pitch);

	/**
	 * @brief 再生する音声ファイルのパスを設定します。
	 */
	void SetAudioPath(const std::string& _path);

	/**
	 * @brief 再生するオーディオクリップアセットを設定します。
	 */
	void SetAudioClip(Asset::AudioClip* _clip);

	/// ----- getter ----- ///

	/**
	 * @brief 設定されている音量を取得します。
	 */
	float GetVolume() const;

	/**
	 * @brief 設定されているピッチを取得します。
	 */
	float GetPitch() const;

	/**
	 * @brief ループ再生設定を取得します。
	 */
	bool GetLoop() const { return isLoop_; }

	/**
	 * @brief ループ再生設定を設定します。
	 */
	void SetLoop(bool _loop) { isLoop_ = _loop; }

	/**
	 * @brief 音声ファイルのパスを取得します。
	 */
	const std::string& GetAudioPath() const;

	/**
	 * @brief 設定されているオーディオクリップアセットを取得します。
	 */
	Asset::AudioClip* GetAudioClip() const;

	/**
	 * @brief 現在の再生状態（AudioState）を取得します。
	 */
	int GetState() const;

};

namespace ComponentDebug {
	/**
	 * @brief エディタ用：AudioSourceコンポーネントのデバッグ表示（Gui描画等）処理を行います。
	 */
	void AudioSourceDebug(AudioSource* _as);
}

namespace MonoInternalMethods {
	/**
	 * @brief C#（Mono）インターフェース用：音量・ピッチパラメータを取得
	 */
	void InternalGetParams(uint64_t _nativeHandle, float* _volume, float* _pitch);

	/**
	 * @brief C#（Mono）インターフェース用：音量・ピッチパラメータを設定
	 */
	void InternalSetParams(uint64_t _nativeHandle, float _volume, float _pitch);

	/**
	 * @brief C#（Mono）インターフェース用：音声の再生開始
	 */
	void InternalPlay(uint64_t _nativeHandle);

	/**
	 * @brief C#（Mono）インターフェース用：音声の停止
	 */
	void InternalStop(uint64_t _nativeHandle);

	/**
	 * @brief C#（Mono）インターフェース用：ワンショット音声の再生要求
	 */
	void InternalPlayOneShot(uint64_t _nativeHandle, float _volume, float _pitch, MonoString* _path);
}



/**
 * @brief JSONからのデシリアライズ
 */
void from_json(const nlohmann::json& _j, AudioSource& _a);

/**
 * @brief JSONへのシリアライズ
 */
void to_json(nlohmann::json& _j, const AudioSource& _a);

} /// ONEngine
