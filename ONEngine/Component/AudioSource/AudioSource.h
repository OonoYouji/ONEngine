#pragma once

#include <xaudio2.h>
#include <string>
#include <list>
#include <memory>
#include <cmath>

#include <Component/Base/BaseComponent.h>	

class AudioClip;


/// ===================================================
/// 音源の再生クラス
/// ===================================================
class AudioSource final : public BaseComponent {
public:

	AudioSource() {}
	~AudioSource() {}
	
	/// ===================================================
	/// public : methods
	/// ===================================================

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;

	void Debug() override;


	/// <summary>
	/// 再生
	/// </summary>
	void PlayAudio();

	/// <summary>
	/// 停止
	/// </summary>
	void StopAudio();

	/// <summary>
	/// 再生中のすべての音を止める
	/// </summary>
	void StopAudioAll();

	/// <summary>
	/// 一個でも再生していたらtrueをカ返す
	/// </summary>
	/// <returns></returns>
	bool IsPlayingAudio();

	/// <summary>
	/// 音源の設定
	/// </summary>
	/// <param name="filePath"></param>
	void SetAudioClip(const std::string& filePath);


	/// <summary>
	/// 一回きりの効果音再生
	/// </summary>
	/// <param name="filePath"></param>
	/// <param name="volume"></param>
	void PlayOneShot(const std::string& filePath, float volume);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	AudioClip* clip_ = nullptr;

	struct Element {
		IXAudio2SourceVoice* pSourceVoice = nullptr;
		bool operator== (const Element&) const = default;
	};

	std::list<Element> sources_;

public:
	/// ===================================================
	/// public : objects
	/// ===================================================

	float volume = 0.5f;
	float pitch = 1.0f;
	bool isLoop = false;
};
