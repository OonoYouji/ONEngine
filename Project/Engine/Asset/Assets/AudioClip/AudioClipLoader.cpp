#include "AudioClipLoader.h"

/// std
#include <fstream>
#include <algorithm>

/// sound api
#include <mfapi.h>
#include <mfobjects.h>
#include <mfidl.h>
#include <mfreadwrite.h>

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Asset/Meta/MetaFile.h"

/// comment
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")


namespace ONEngine::Asset {

/**
 * @brief ディスクからWAV形式の音声ファイルをロードします。
 * @param _filepath ロード対象のファイルパス
 * @param meta 音声のメタデータ
 * @return ロードされたAudioClipアセット（失敗時はstd::nullopt）
 */
std::optional<AudioClip> AssetLoader<AudioClip>::Load(const std::string& _filepath, Meta<AudioClip::MetaData> meta) {
	/// ----- オーディオクリップの読み込み ----- ///

	/// Media Foundationの初期化 (一度だけ実行)
	static HRESULT mfStartupResult = MFStartup(MF_VERSION);
	if (!SUCCEEDED(mfStartupResult)) {
		Console::LogError("[Load Failed] [AudioClip] - MFStartup failed.");
		return std::nullopt;
	}

	/// ファイルが存在するのかチェックする
	if(!std::filesystem::exists(_filepath)) {
		Console::LogError("[Load Failed] [AudioClip] - File not found: \"" + _filepath + "\"");
		return std::nullopt;
	}

	/// パスをWindows形式（バックスラッシュ）に変換
	std::string fixedPath = _filepath;
	std::replace(fixedPath.begin(), fixedPath.end(), '/', '\\');

	/// wstringに変換
	std::wstring filePathW = ConvertString(fixedPath);
	HRESULT result;

	/// SourceReaderの作成
	ComPtr<IMFSourceReader> sourceReader;
	result = MFCreateSourceReaderFromURL(filePathW.c_str(), nullptr, &sourceReader);
	if(!SUCCEEDED(result)) {
		Console::LogError(std::format("[Load Failed] [AudioClip] - MFCreateSourceReaderFromURL failed (HRESULT: 0x{:08X}): \"{}\"", (uint32_t)result, _filepath));
		return std::nullopt;
	}

	/// PCM形式にフォーマットを指定する
	ComPtr<IMFMediaType> audioType;
	MFCreateMediaType(&audioType);
	audioType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	audioType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	result = sourceReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, audioType.Get());
	if(!SUCCEEDED(result)) {
		Console::LogError("[Load Failed] [AudioClip] - SetCurrentMediaType failed: \"" + _filepath + "\"");
		return std::nullopt;
	}

	/// メディアタイプの取得
	ComPtr<IMFMediaType> pOutType;
	sourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pOutType);

	/// waveフォーマットの取得
	WAVEFORMATEX* waveFormat = nullptr;
	MFCreateWaveFormatExFromMFMediaType(pOutType.Get(), &waveFormat, nullptr);


	/// コンテナに格納する用のデータを作成する
	AudioStructs::SoundData soundData{};
	soundData.wfex = *waveFormat;

	/// 作成したwaveフォーマットを解放
	CoTaskMemFree(waveFormat);

	/// PCMデータのバッファを構築
	while(true) {
		ComPtr<IMFSample> pSample;
		DWORD streamIndex = 0;
		DWORD flags = 0;
		LONGLONG llTimeStamp = 0;

		/// サンプルを読み込む
		result = sourceReader->ReadSample(
			(DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM,
			0,
			&streamIndex, &flags,
			&llTimeStamp, &pSample
		);

		/// ストリームが終了したら抜ける
		if(flags & MF_SOURCE_READERF_ENDOFSTREAM) {
			break;
		}

		if(pSample) {
			ComPtr<IMFMediaBuffer> pBuffer;
			/// サンプルに含まれるサウンドデータのバッファを一繋ぎにして取得
			pSample->ConvertToContiguousBuffer(&pBuffer);

			BYTE* pData = nullptr;
			DWORD maxLength = 0;
			DWORD currentLength = 0;
			/// バッファ読み込み用にロック
			pBuffer->Lock(&pData, &maxLength, &currentLength);
			/// バッファの末尾にデータを追加
			soundData.buffer.insert(soundData.buffer.end(), pData, pData + currentLength);
			pBuffer->Unlock();

		}

	}

	AudioClip audioClip;
	audioClip.guid = meta.base.guid;
	audioClip.soundData_ = std::move(soundData);

	Console::Log("[Load] [AudioClip] - path:\"" + _filepath + "\"");

	return std::move(audioClip);
}

/**
 * @brief 既存のAudioClipアセットに対して再ロード（リロード）を実行します。
 * @param _filepath 再ロード対象のファイルパス
 * @param _src 再ロード元のAudioClipオブジェクトへのポインタ
 * @param meta 音声のメタデータ
 * @return 再ロードされたAudioClipアセット（失敗時はstd::nullopt）
 */
std::optional<AudioClip> AssetLoader<AudioClip>::Reload(const std::string& _filepath, AudioClip* /*_src*/, Meta<AudioClip::MetaData> meta) {
	return std::move(Load(_filepath, meta));
}


/**
 * @brief 音声アセットに対応するメタデータを取得します。
 * @param _filepath 対象アセットファイルのパス
 * @return 解析・構築されたメタデータオブジェクト
 */
Meta<AudioClip::MetaData> AssetLoader<AudioClip>::GetMetaData(const std::string& _filepath) {
	Meta<AudioClip::MetaData> res{};

	const std::string metaPath = _filepath + ".meta";
	res.base = LoadOrGenerateMetaBase(metaPath, _filepath);

	nlohmann::json j;
	std::ifstream ifs(metaPath);
	if(!ifs.is_open()) {
		return {};
	}

	ifs >> j;
	AudioClip::MetaData data;
	data.duration = j.value("duration", 0.0f);

	res.data = data;

	return res;
}


} /// namespace ONEngine::Asset