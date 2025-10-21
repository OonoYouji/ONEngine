#pragma once

/// std
#include <cstdint>
#include <vector>

#include <xaudio2.h>

/// ////////////////////////////////////////////////////////////////
/// @brief サウンドの読み込み、再生に必用な構造体
/// ////////////////////////////////////////////////////////////////
namespace AudioStructs {

	struct ChunkHeader {
		char         id[4];      /// チャンク毎のID
		int32_t      size;       /// チャンクサイズ
	};

	struct RiffHeader {
		ChunkHeader  chunk;      /// "RIFF"
		char         type[4];    /// "WAVE"
	};

	struct FormatChunk {
		ChunkHeader  chunk;      /// "fmt"
		WAVEFORMATEX fmt;        /// 波形フォーマット
	};

	struct SoundData {
		WAVEFORMATEX wfex;       /// 波形フォーマット
		std::vector<BYTE> buffer; /// 音声データ
	};
}


/// ////////////////////////////////////////////////////////////////
/// 音のサウンドデータ
/// ////////////////////////////////////////////////////////////////
class AudioClip {
	friend class AssetLoader;
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	AudioClip() = default;
	~AudioClip();

	/// サウンドデータの作成
	IXAudio2SourceVoice* CreateSourceVoice(IXAudio2* _audio);

	/// サウンドデータの取得
	const AudioStructs::SoundData& GetSoundData();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	AudioStructs::SoundData soundData_;

};


void SoundDataUnload(AudioStructs::SoundData* _soundData);

