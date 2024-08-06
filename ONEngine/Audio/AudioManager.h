#pragma once

#include <wrl/client.h>
#include <d3d12.h>

#include <unordered_map>
#include <string>

#include <Audio.h>


using namespace Microsoft::WRL;




/// ===================================================
/// 音の管理クラス
/// ===================================================
class AudioManager final {
	AudioManager() = default;
	~AudioManager() = default;
public:

	static AudioManager* GetInstance();

	void Initialize();


	void Load(const std::string& filePath);


private:

	AudioClip LoadAudioClip(const std::string& filePath);

private:

	const std::string kDirectoryPath_ = "./Resources/Audios/";

	ComPtr<IXAudio2> xAudio2_ = nullptr;
	IXAudio2MasteringVoice* masterVoice_ = nullptr;

	std::unordered_map<std::string, AudioClip> clips_;

private:
	AudioManager(const AudioManager&) = delete;
	AudioManager(AudioManager&&) = delete;
	AudioManager& operator= (const AudioManager&) = delete;
	AudioManager& operator= (AudioManager&&) = delete;
}; 