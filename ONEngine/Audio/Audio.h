#pragma once

#include <xaudio2.h>

class AudioSource final {
public:

	AudioSource() {};
	~AudioSource() {};

	void PlayAudio();
	void StopAudio();
	void PauseAudio();

private:

	//AudioClip* clip_ = nullptr;

};



/// <summary>
/// 音声データ
/// </summary>
struct AudioClip {
	WAVEFORMATEX wfex;			//- 波形フォーマット
	BYTE* pBuffer;				//- バッファの先頭アドレス
	unsigned int bufferSize;	//- バッファのサイズ
	IXAudio2SourceVoice* pSourceVoice;
};

