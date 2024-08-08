#include <AudioManager.h>

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

	for(auto& audioSource : sources_) {
		audioSource.reset();
	}

	xAudio2_.Reset();
}


void AudioManager::Update() {
	for(auto& source : sources_) {
		source->Update();
	}
}


void AudioManager::Load(const std::string& filePath) {

	/// mapに存在するか確認
	if(clips_.find(filePath) != clips_.end()) {
		assert(false);
		return;
	}

	/// ファイルが存在するか確認
	std::ifstream file(kDirectoryPath_ + filePath);
	if(!file.good()) {
		assert(false);
		return;
	}

	clips_[filePath] = LoadWave(kDirectoryPath_ + filePath);
}


AudioClip* AudioManager::GetAudioClip(const std::string& filePath) {
	return &clips_.at(filePath);
}

void AudioManager::AddAudioSource(AudioSource* source) {
	sources_.push_back(std::move(std::unique_ptr<AudioSource>(source)));
}

void AudioManager::SubAudioSource(AudioSource* source) {
	auto it = std::find_if(sources_.begin(), sources_.end(),
						   [source](const std::unique_ptr<AudioSource>& obj) {
		return obj.get() == source;
	});

	if(it != sources_.end()) {
		sources_.erase(it);
	}
}




AudioClip AudioManager::LoadWave(const std::string& filePath) {
	HRESULT hr = S_FALSE;

	///- ファイルオープン
	std::ifstream file;
	file.open(filePath, std::ios_base::binary); //- .wavをバイナリモードで開く
	assert(file.is_open());


	///- .wavデータの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));

	///- ファイルがRIFFかどうか確認する
	if(strncmp(riff.chunk.id, "RIFF", 4) != 0) { assert(false); }
	///- ファイルがWAVEかチェック
	if(strncmp(riff.type, "WAVE", 4) != 0) { assert(false); }

	FormatChunk format{};
	file.read((char*)&format, sizeof(ChunkHeader));

	///- チャンクヘッダの確認
	if(strncmp(format.chunk.id, "fmt ", 4) != 0) { assert(false); }

	///- チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	ChunkHeader data;
	file.read((char*)&data, sizeof(data));

	///- JUNKチャンクを検出した場合
	if(strncmp(data.id, "JUNK", 4) == 0) {

		file.seekg(data.size, std::ios_base::cur);
		file.read((char*)&data, sizeof(data));
	}

	if(strncmp(data.id, "data", 4) != 0) { assert(false); }

	///- Dataチャンクのデータ部(波形データ)の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	///- ファイルクローズ
	file.close();

	///- 読み込んだ音声データをreturn
	AudioClip soundData{};
	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;

	return soundData;
}

