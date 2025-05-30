#pragma once

/// std
#include <string>

/// engine
#include "../../Interface/IComponent.h"

/// ////////////////////////////////////////////////////////////
/// Audio Source
/// ////////////////////////////////////////////////////////////
class AudioSource : public IComponent {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================
	AudioSource() {}
	~AudioSource() {}

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	float volume_ = 1.0f;
	float pitch_ = 1.0f;

	std::string path_;

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/// @brief 音量の設定
	/// @param _volume 音量
	void SetVolume(float _volume) { volume_ = _volume; }

	/// @brief 音の高さの設定
	/// @param _pitch 音の高さ
	void SetPitch(float _pitch) { pitch_ = _pitch; }

	/// @brief 音源のパスの設定
	/// @param _path 音源のパス
	void SetAudioPath(const std::string& _path) { path_ = _path; }



	/// @brief 音量の取得
	/// @return 音量
	float GetVolume() const { return volume_; }

	/// @brief 音の高さの取得
	/// @return 音の高さ
	float GetPitch() const { return pitch_; }

	/// @brief 音源のパスの取得
	/// @return 音源のパス
	const std::string& GetAudioPath() const { return path_; }

};

