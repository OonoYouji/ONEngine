#pragma once

/// std
#include <string>

/// audio
#include <xaudio2.h>

/// external
#include <nlohmann/json.hpp>
#include <mono/jit/jit.h>

/// engine
#include "../../Interface/IComponent.h"


/// @brief 音の再生状態
enum AudioState {
	AudioState_Stopped,
	AudioState_Playing,
	AudioState_Paused,
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
class AudioSource : public IComponent {
	friend class AudioPlaybackSystem;
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	AudioSource();
	~AudioSource();

	/// 再生
	void Play();
	void PlayOneShot(float _volume, float _pitch, const std::string& _path);

	/// 追加
	void AddSourceVoice(IXAudio2SourceVoice* _sourceVoice);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	class AudioClip* pAudioClip_;
	std::string path_;
	float volume_;
	float pitch_;

	int state_;
	bool isPlayingRequest_;

	/// 再生中の音声ソースリスト
	std::list<IXAudio2SourceVoice*> sourceVoices_;

	/// ワンショット再生リクエストリスト
	std::vector<OneShotAudio> oneShotAudioRequests_;

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/// ----- setter ----- ///
	void SetVolume(float _volume);
	void SetPitch(float _pitch);
	void SetAudioPath(const std::string& _path);
	void SetAudioClip(class AudioClip* _clip);

	/// ----- getter ----- ///
	float GetVolume() const;
	float GetPitch() const;
	const std::string& GetAudioPath() const;
	class AudioClip* GetAudioClip() const;
	int GetState() const;

};



namespace COMP_DEBUG {
	void AudioSourceDebug(AudioSource* _as);
}

void from_json(const nlohmann::json& _j, AudioSource& _a);
void to_json(nlohmann::json& _j, const AudioSource& _a);


namespace MONO_INTERNAL_METHOD {
	void InternalGetParams(uint64_t _nativeHandle, float* _volume, float* _pitch);
	void InternalSetParams(uint64_t _nativeHandle, float _volume, float _pitch);
	void InternalPlayOneShot(uint64_t _nativeHandle, float _volume, float _pitch, MonoString* _path);
}
