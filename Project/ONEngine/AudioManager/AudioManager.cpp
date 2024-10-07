#include "AudioManager.h"

#include <fstream>
#include <cassert>

#pragma comment (lib, "xaudio2.lib")



namespace {

	/// <summary>
	/// チャンクヘッダ
	/// </summary>
	struct ChunkHeader {
		char id[4];			//- チャンク毎のID
		int32_t size;		//- チャンクサイズ
	};

	/// <summary>
	/// RIFFヘッダチャンク
	/// </summary>
	struct RiffHeader {
		ChunkHeader chunk;	//- "RIFF"
		char type[4];		//- "WAVE"
	};

	/// <summary>
	/// FMTチャンク
	/// </summary>
	struct FormatChunk {
		ChunkHeader chunk;	//- "fmt"
		WAVEFORMATEX fmt;	//- 波形フォーマット
	};

}


/// ===================================================
/// 初期化
/// ===================================================
void AudioManager::Initialize() {
	HRESULT hr = S_FALSE;

	///- xAudioインスタンスの生成
	hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));

	///- MasterVoiceの生成
	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(hr));

}

void AudioManager::Finalize() {
	for(auto& audioClip : clips_) {
		audioClip.second.Release();
	}

	xAudio2_.Reset();
}


void AudioManager::Update() {
	
}


void AudioManager::Load(const std::string& filePath) {
	AudioManager* instance = GetInstance();

	/// mapに存在するか確認
	if(instance->clips_.find(filePath) != instance->clips_.end()) {
		assert(false);
		return;
	}

	/// ファイルが存在するか確認
	std::ifstream file(instance->kDirectoryPath_ + filePath);
	if(!file.good()) {
		assert(false);
		return;
	}

	instance->clips_[filePath] =
		instance->LoadWave(instance->kDirectoryPath_ + filePath);
}


AudioClip* AudioManager::GetAudioClip(const std::string& filePath) {
	if(clips_.find(filePath) == clips_.end()) {
		Load(filePath);
	}

	return &clips_.at(filePath);
}




AudioClip AudioManager::LoadWave(const std::string& filePath) {
	AudioClip audioClip{};

	std::ifstream file(filePath, std::ios::binary);
	if(!file) {
		assert(false);
	}

	// WAVヘッダの読み込み
	char chunkId[4];
	file.read(chunkId, 4);
	if(std::strncmp(chunkId, "RIFF", 4) != 0) {
		assert(false);
	}

	file.ignore(4); // ChunkSizeをスキップ

	file.read(chunkId, 4);
	if(std::strncmp(chunkId, "WAVE", 4) != 0) {
		assert(false);
	}

	// fmtチャンクを探す
	bool fmtFound = false;
	while(file.read(chunkId, 4)) {
		uint32_t chunkSize;
		file.read(reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize));

		if(std::strncmp(chunkId, "fmt ", 4) == 0) {
			fmtFound = true;
			if(chunkSize >= sizeof(WAVEFORMATEX)) {
				file.read(reinterpret_cast<char*>(&audioClip.wfex), sizeof(WAVEFORMATEX));

				// fmtChunkSize が WAVEFORMATEX よりも大きい場合は、余分なデータをスキップ
				if(chunkSize > sizeof(WAVEFORMATEX)) {
					file.ignore(chunkSize - sizeof(WAVEFORMATEX));
				}
			} else if(chunkSize == 40) {
				file.read(reinterpret_cast<char*>(&audioClip.wfex), sizeof(WAVEFORMATEX));
				// 追加の16バイト（WAVEFORMATEXを超える部分）をスキップ
				file.ignore(chunkSize - sizeof(WAVEFORMATEX));
			} else if (chunkSize == 16) {
				// 16バイトのfmtチャンクの場合
				file.read(reinterpret_cast<char*>(&audioClip.wfex), 16);
			}
			break;
		} else {
			// チャンクをスキップ
			file.ignore(chunkSize);
		}
	}

	if(!fmtFound) {
		assert(false);
	}

	// dataチャンクまでスキップ
	file.read(chunkId, 4);
	while(std::strncmp(chunkId, "data", 4) != 0) {
		uint32_t chunkSize;
		file.read(reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize));
		file.ignore(chunkSize);
		file.read(chunkId, 4);
	}

	uint32_t dataSize;
	file.read(reinterpret_cast<char*>(&dataSize), sizeof(dataSize));

	audioClip.pBuffer = new BYTE[dataSize];
	file.read(reinterpret_cast<char*>(audioClip.pBuffer), dataSize);

	audioClip.bufferSize = dataSize;

	file.close();
	return audioClip;
}

