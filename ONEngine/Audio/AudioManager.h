#pragma once

#include <wrl/client.h>
#include <d3d12.h>

#include <unordered_map>
#include <string>

#include <AudioClip.h>
#include <AudioSource.h>


using namespace Microsoft::WRL;




/// ===================================================
/// 音の管理クラス
/// ===================================================
class AudioManager final {
	AudioManager() = default;
	~AudioManager() = default;
public:


	/// <summary>
	/// インスタンス確保
	/// </summary>
	static AudioManager* GetInstance() {
		static AudioManager instance;
		return &instance;
	}


	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();


	/// <summary>
	/// 更新
	/// </summary>
	void Update();


	/// <summary>
	/// 音声データの読み込み
	/// </summary>
	/// <param name="filePath"> : "./Resources/Audios/" + filePath </param>
	static void Load(const std::string& filePath);

	static void ResetAudioSources();



	void AddAudioSource(AudioSource* source);

	void SubAudioSource(AudioSource* source);



	AudioClip* GetAudioClip(const std::string& filePath);

	IXAudio2* GetxAudio2() const { return xAudio2_.Get(); }

private:

	AudioClip LoadWave(const std::string& filePath);


private:

	/// Audiosまでのファイルパス
	const std::string kDirectoryPath_ = "./Resources/Audios/";

	ComPtr<IXAudio2> xAudio2_ = nullptr;
	IXAudio2MasteringVoice* masterVoice_ = nullptr;

	/// 音源データのコンテナ
	std::unordered_map<std::string, AudioClip> clips_;

	/// 音源の再生クラスコンテナ
	std::list<std::unique_ptr<AudioSource>> sources_;

private:
	AudioManager(const AudioManager&) = delete;
	AudioManager(AudioManager&&) = delete;
	AudioManager& operator= (const AudioManager&) = delete;
	AudioManager& operator= (AudioManager&&) = delete;
}; 