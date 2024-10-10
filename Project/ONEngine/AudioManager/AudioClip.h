#pragma once

#include <xaudio2.h>

/// ===================================================
/// 音源データ
/// ===================================================
class AudioClip final {
public:

	AudioClip() {};
	~AudioClip() {};

	/// <summary>
	/// データの解放
	/// </summary>
	void Release();

	
	IXAudio2SourceVoice* CreateSourceVoice();


public:

	WAVEFORMATEX wfex;			//- 波形フォーマット
	BYTE* pBuffer;				//- バッファの先頭アドレス
	unsigned int bufferSize;	//- バッファのサイズ
};