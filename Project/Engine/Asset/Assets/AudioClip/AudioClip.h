#pragma once

/// std
#include <cstdint>
#include <vector>

#include <xaudio2.h>

/// engine
#include "../IAsset.h"



namespace ONEngine::Asset {
template<typename T>
class AssetLoader;
}



namespace ONEngine::Asset {


/**
 * @namespace AudioStructs
 * @brief WAV波形フォーマットの解析や音声データの保持に使用する構造体定義
 */
namespace AudioStructs {
	/**
	 * @struct ChunkHeader
	 * @brief RIFFファイルのチャンク共通ヘッダー
	 */
	struct ChunkHeader {
		char         id[4];  ///< チャンクのID（"RIFF", "fmt ", "data" 等）
		int32_t      size;   ///< チャンクのデータサイズ
	};

	/**
	 * @struct RiffHeader
	 * @brief RIFF WAVEファイルの最上位ヘッダー
	 */
	struct RiffHeader {
		ChunkHeader  chunk;  ///< RIFFヘッダーチャンク
		char         type[4];///< ファイルタイプ（"WAVE"）
	};

	/**
	 * @struct FormatChunk
	 * @brief WAVファイルの音声フォーマット情報（サンプリングレート、チャンネル数等）を格納するチャンク
	 */
	struct FormatChunk {
		ChunkHeader  chunk;  ///< fmtチャンクヘッダー
		WAVEFORMATEX fmt;    ///< 波形フォーマット詳細
	};

	/**
	 * @struct SoundData
	 * @brief ロード済みの生波形フォーマットと音声バイナリデータを保持する構造体
	 */
	struct SoundData {
		WAVEFORMATEX wfex;        ///< 波形フォーマット
		std::vector<BYTE> buffer; ///< 音声バイナリデータ
	};
}


/**
 * @class AudioClip
 * @brief 再生可能な音声（WAV）アセットを表すクラス
 */
class AudioClip final : public IAsset {
	friend class AssetLoader<AudioClip>;
public:

	/**
	 * @struct MetaData
	 * @brief AudioClip固有のメタデータ（再生時間等）を保持する構造体
	 */
	struct MetaData {
		float duration; ///< 音声の長さ（秒）
	};


public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief デフォルトコンストラクタ
	 */
	AudioClip() = default;

	/**
	 * @brief デストラクタ。音声データのバッファを解放します。
	 */
	~AudioClip();

	/**
	 * @brief 指定されたXAudio2デバイス上にソースボイスを構築し、波形データをバインドします。
	 * @param _audio IXAudio2デバイスへのポインタ
	 * @return 構築された IXAudio2SourceVoice へのポインタ（構築失敗時はnullptr）
	 */
	IXAudio2SourceVoice* CreateSourceVoice(IXAudio2* _audio);

	/**
	 * @brief ロードされているサウンドデータの構造体を取得します。
	 * @return SoundDataへの参照
	 */
	const AudioStructs::SoundData& GetSoundData();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	AudioStructs::SoundData soundData_; ///< WAV音声の実データ

};


/**
 * @brief ロード済みの音声データを明示的に解放しアンロードします。
 * @param _soundData アンロードするSoundDataのポインタ
 */
void SoundDataUnload(AudioStructs::SoundData* _soundData);


} /// namespace ONEngine::Asset
