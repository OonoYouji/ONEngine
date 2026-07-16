#include "AudioClip.h"

/// engine
#include "Engine/Core/Utility/Utility.h"


namespace ONEngine::Asset {

/**
 * @brief デストラクタ。サウンドデータをアンロードします。
 */
AudioClip::~AudioClip() {
	/// 音データの解放
	SoundDataUnload(&soundData_);
}

/**
 * @brief 音声ソースを作成し返します。
 * @param _audio IXAudio2デバイスへのポインタ
 * @return 構築された IXAudio2SourceVoice へのポインタ
 */
IXAudio2SourceVoice* AudioClip::CreateSourceVoice(IXAudio2* _audio) {
	/// 音声ソースを作成し返す
	IXAudio2SourceVoice* sourceVoice = nullptr;
	HRESULT result = _audio->CreateSourceVoice(&sourceVoice, &soundData_.wfex);
	Assert(SUCCEEDED(result));
	return sourceVoice;
}

/**
 * @brief 音声の実データを取得します。
 * @return SoundDataへの参照
 */
const AudioStructs::SoundData& AudioClip::GetSoundData() {
	return soundData_;
}

/**
 * @brief 音声の実データバッファをクリアしアンロードします。
 * @param _soundData 対象のSoundDataポインタ
 */
void SoundDataUnload(AudioStructs::SoundData* _soundData) {
	/// 音データの解放をする
	_soundData->buffer.clear();
	_soundData->wfex = {};
}

} /// namespace ONEngine::Asset