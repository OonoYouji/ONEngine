#pragma once

#include <xaudio2.h>
#include <string>

class AudioClip;



/// ===================================================
/// 音源の再生クラス
/// ===================================================
class AudioSource final {
public:

	AudioSource() {};
	~AudioSource() {};

	void PlayAudio();
	void StopAudio();
	void PauseAudio();

	void SetAudioClip(const std::string& filePath);

private:

	AudioClip* clip_ = nullptr;

	struct Element {
		float volume = 0.5f;
		IXAudio2SourceVoice* pSourceVoice = nullptr;
	};

	Element elem;

};
