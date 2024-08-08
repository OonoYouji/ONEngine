#pragma once

#include <xaudio2.h>
#include <string>
#include <list>
#include <memory>
#include <cmath>

class AudioClip;


/// ===================================================
/// 音のタイプ
/// ===================================================
enum class AudioType : int8_t {
	SE,		/// sound effect
	BGM		/// background music
};

/// ===================================================
/// 音源の再生クラス
/// ===================================================
class AudioSource final {
public:

	AudioSource() {};
	~AudioSource() {};


	static inline std::unique_ptr<AudioSource> MakeInstance() {
		std::unique_ptr<AudioSource> newInstance(new AudioSource);
		return std::move(newInstance);
	}

	void Update();

	void FadeIn();
	void FadeOut();


	/// <summary>
	/// 再生
	/// </summary>
	void PlayAudio();

	/// <summary>
	/// 停止
	/// </summary>
	void StopAudio();


	void StopAudioALL();


	/// <summary>
	/// 一時停止
	/// </summary>
	/*void PauseAudio();

	void UnPouseAudio();*/

	/// <summary>
	/// 音源の設定
	/// </summary>
	/// <param name="filePath"></param>
	void SetAudioClip(const std::string& filePath);

private:

	AudioClip* clip_ = nullptr;

	struct Element {
		IXAudio2SourceVoice* pSourceVoice = nullptr;
	};

	std::list<Element> sources_;
	float volume_ = 0.5f;
	float pitch_ = 1.0f;

	bool isLoop_ = false;

};
