#pragma once

/// std
#include <string>

/// external
#include <nlohmann/json.hpp>

/// engine
#include "../../Interface/IComponent.h"


/// @brief 音の再生状態
enum AudioState {
	AudioState_Stopped,
	AudioState_Playing,
	AudioState_Paused,
};


/// ////////////////////////////////////////////////////////////
/// Audio Source
/// ////////////////////////////////////////////////////////////
class AudioSource : public IComponent {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	AudioSource();
	~AudioSource();

	/// 再生
	void Play();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::string path_;
	float volume_;
	float pitch_;

	int state_;

	bool isPlayingRequest_;
	

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/// ----- setter ----- ///
	void SetVolume(float _volume);
	void SetPitch(float _pitch);
	void SetAudioPath(const std::string& _path);


	/// ----- getter ----- ///
	float GetVolume() const;
	float GetPitch() const;
	const std::string& GetAudioPath() const;

};



/// json serialize
void to_json(nlohmann::json& _j, const AudioSource& _as);
void from_json(const nlohmann::json& _j, AudioSource& _as);