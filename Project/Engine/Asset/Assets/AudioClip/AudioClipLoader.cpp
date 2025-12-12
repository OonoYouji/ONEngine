#include "AudioClipLoader.h"

/// sound api
#include <mfapi.h>
#include <mfobjects.h>
#include <mfidl.h>
#include <mfreadwrite.h>

/// engine
#include "Engine/Core/Utility/Utility.h"

/// comment
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

using namespace ONEngine;

std::optional<AudioClip> AssetLoader<AudioClip>::Load(const std::string& _filepath) {
	/// ----- オーディオクリップの読み込み ----- ///

	/// ファイルが存在するのかチェックする
	if (!std::filesystem::exists(_filepath)) {
		Console::LogError("[Load Failed] [AudioClip] - File not found: \"" + _filepath + "\"");
		return std::nullopt;
	}

	/// wstringに変換
	std::wstring filePathW = ConvertString(_filepath);
	HRESULT result;

	/// SourceReaderの作成
	ComPtr<IMFSourceReader> sourceReader;
	result = MFCreateSourceReaderFromURL(filePathW.c_str(), nullptr, &sourceReader);
	if (!SUCCEEDED(result)) {
		Console::LogError("[Load Failed] [AudioClip] - MFCreateSourceReaderFromURL failed: \"" + _filepath + "\"");
		return std::nullopt;
	}

	/// PCM形式にフォーマットを指定する
	ComPtr<IMFMediaType> audioType;
	MFCreateMediaType(&audioType);
	audioType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	audioType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	result = sourceReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, audioType.Get());
	if (!SUCCEEDED(result)) {
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
	while (true) {
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
		if (flags & MF_SOURCE_READERF_ENDOFSTREAM) {
			break;
		}

		if (pSample) {
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
	audioClip.soundData_ = std::move(soundData);

	Console::Log("[Load] [AudioClip] - path:\"" + _filepath + "\"");

	return std::move(audioClip);
}

std::optional<AudioClip> AssetLoader<AudioClip>::Reload(const std::string& _filepath, AudioClip* /*_src*/) {
	return std::move(Load(_filepath));
}