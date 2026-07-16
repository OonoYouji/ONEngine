#include "AudioSource.h"

/// external
#include <imgui.h>
#include <magic_enum/magic_enum.hpp>

/// engine
#include "Engine/Asset/Assets/AudioClip/AudioClip.h"
#include "Engine/Core/Utility/Utility.h"

/// editor
#include "Engine/Editor/Math/ImGuiMath.h"
#include "Engine/Editor/Math/AssetPayload.h"

using namespace ONEngine;

/**
 * @brief コンストラクタ
 */
AudioSource::AudioSource()
	: volume_(1.0f),
	pitch_(1.0f),
	state_(0),
	isPlayingRequest_(false) {
}

/**
 * @brief デストラクタ
 */
AudioSource::~AudioSource() {}

/**
 * @brief 音声ファイルの再生を開始（または一時停止から復帰）します。
 */
void AudioSource::Play() {
	isPlayingRequest_ = true;
}

/**
 * @brief 音声ファイルの再生を停止します。
 */
void AudioSource::Stop() {
	isStopRequest_ = true;
}

/**
 * @brief 指定されたパスの音声（効果音等）を指定音量・ピッチで一度限り重複再生（ワンショット再生）します。
 */
void AudioSource::PlayOneShot(float _volume, float _pitch, const std::string& _path) {
	oneShotAudioRequests_.push_back({ _path, _volume, _pitch });
}

/**
 * @brief 再生対象となるXAudio2のソースボイスを追加登録します。
 */
void AudioSource::AddSourceVoice(IXAudio2SourceVoice* _sourceVoice) {
	sourceVoices_.push_back(_sourceVoice);
}

/**
 * @brief 音量を設定します。
 */
void AudioSource::SetVolume(float _volume) {
	volume_ = _volume;
}

/**
 * @brief 再生ピッチ（周波数比）を設定します。
 */
void AudioSource::SetPitch(float _pitch) {
	pitch_ = _pitch;
}

/**
 * @brief 再生する音声ファイルのパスを設定します。
 */
void AudioSource::SetAudioPath(const std::string& _path) {
	path_ = _path;
}

/**
 * @brief 再生するオーディオクリップアセットを設定します。
 */
void AudioSource::SetAudioClip(Asset::AudioClip* _clip) {
	pAudioClip_ = _clip;
}

/**
 * @brief 設定されている音量を取得します。
 */
float AudioSource::GetVolume() const {
	return volume_;
}

/**
 * @brief 設定されているピッチを取得します。
 */
float AudioSource::GetPitch() const {
	return pitch_;
}

/**
 * @brief 音声ファイルのパスを取得します。
 */
const std::string& AudioSource::GetAudioPath() const {
	return path_;
}

/**
 * @brief 設定されているオーディオクリップアセットを取得します。
 */
Asset::AudioClip* AudioSource::GetAudioClip() const {
	return pAudioClip_;
}

/**
 * @brief 現在の再生状態（AudioState）を取得します。
 */
int AudioSource::GetState() const {
	return state_;
}


/// 

/**
 * @brief エディタ用：AudioSourceコンポーネントのデバッグ表示（Gui描画等）処理を行います。
 */
void ComponentDebug::AudioSourceDebug(AudioSource* _as) {
	if (!_as) {
		return;
	}

	std::string audioPath = _as->GetAudioPath();

	/// audio clipの編集
	ImGui::Text("Audio Source");
	Editor::ImMathf::InputText("Audio Path", &audioPath, ImGuiInputTextFlags_ReadOnly);
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetData")) {
			if (payload->Data) {
				Editor::AssetPayload* assetPayload = *static_cast<Editor::AssetPayload**>(payload->Data);
				std::string path = assetPayload->filePath;
				std::string extension = FileSystem::FileExtension(path);

				/// Audioのパスが有効な形式か確認
				if (extension == ".mp3" ||
					extension == ".wav" ||
					extension == ".ogg") {
					_as->SetAudioPath(path);

					Console::Log(std::format("Audio path set to: {}", path));
				} else {
					Console::LogError("Invalid audio format. Please use .mp3, .wav, or .ogg.");
				}
			}
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::Spacing();

	/// 音量の編集
	float volume = _as->GetVolume();
	ImGui::Text("Volume");
	if (ImGui::SliderFloat("##Volume", &volume, 0.0f, 1.0f, "%.2f")) {
		_as->SetVolume(volume);
	}

	/// ピッチの編集
	float pitch = _as->GetPitch();
	ImGui::Text("Pitch");
	if (ImGui::SliderFloat("##Pitch", &pitch, 0.0f, 3.0f, "%.2f")) {
		_as->SetPitch(pitch);
	}

	ImGui::Spacing();

	/// 再生ボタン
	if (ImGui::Button("Play")) {
		_as->Play();
	}

	ImGui::Spacing();

	/// 再生状態の表示
	int state = _as->GetState();
	std::string stateStr = static_cast<std::string>(magic_enum::enum_name(static_cast<AudioState>(state)));
	ImGui::Text("State: %s", stateStr.c_str());

}

/**
 * @brief C#（Mono）インターフェース用：音量・ピッチパラメータを取得
 */
void MonoInternalMethods::InternalGetParams(uint64_t _nativeHandle, float* _volume, float* _pitch) {
	AudioSource* audioSource = reinterpret_cast<AudioSource*>(_nativeHandle);
	if (!audioSource) {
		Console::LogError("AudioSource pointer is null");
		return;
	}

	*_volume = audioSource->GetVolume();
	*_pitch = audioSource->GetPitch();

}

/**
 * @brief C#（Mono）インターフェース用：音量・ピッチパラメータを設定
 */
void ONEngine::MonoInternalMethods::InternalSetParams(uint64_t _nativeHandle, float _volume, float _pitch) {
	AudioSource* audioSource = reinterpret_cast<AudioSource*>(_nativeHandle);
	if (!audioSource) {
		Console::LogError("AudioSource pointer is null in InternalSetParams");
		return;
	}

	Console::Log(std::format("[CPP Audio] SetParams - Vol: {}, Pitch: {}", _volume, _pitch));
	audioSource->SetVolume(_volume);
	audioSource->SetPitch(_pitch);
}

/**
 * @brief C#（Mono）インターフェース用：音声の再生開始
 */
void ONEngine::MonoInternalMethods::InternalPlay(uint64_t _nativeHandle) {
	AudioSource* audioSource = reinterpret_cast<AudioSource*>(_nativeHandle);
	if (audioSource) {
		Console::Log("[CPP Audio] Play Requested");
		audioSource->Play();
	}
}

/**
 * @brief C#（Mono）インターフェース用：音声の停止
 */
void ONEngine::MonoInternalMethods::InternalStop(uint64_t _nativeHandle) {
	AudioSource* audioSource = reinterpret_cast<AudioSource*>(_nativeHandle);
	if (audioSource) {
		Console::Log("[CPP Audio] Stop Requested");
		audioSource->Stop();
	}
}

/**
 * @brief C#（Mono）インターフェース用：ワンショット音声の再生要求
 */
void ONEngine::MonoInternalMethods::InternalPlayOneShot(uint64_t _nativeHandle, float _volume, float _pitch, MonoString* _path) {
	/// 音の再生
	AudioSource* audioSource = reinterpret_cast<AudioSource*>(_nativeHandle);
	if (!audioSource) {
		Console::LogError("AudioSource pointer is null in InternalPlayOneShot");
		return;
	}

	/// pathの変換
	char* path = mono_string_to_utf8(_path);

	Console::Log(std::format("[CPP Audio] OneShot Requested - Path: {}, Vol: {}, Pitch: {}", path, _volume, _pitch));
	audioSource->PlayOneShot(_volume, _pitch, std::string(path));

	mono_free(path);
}


/// json serialize
/**
 * @brief JSONからのデシリアライズ
 */
void ONEngine::from_json(const nlohmann::json& _j, AudioSource& _a) {
	_a.enable = _j.value("enable", 1);
	_a.SetVolume(_j.value("volume", 1.0f));
	_a.SetPitch(_j.value("pitch", 1.0f));
	_a.SetAudioPath(_j.value("path", std::string("")));
}

/**
 * @brief JSONへのシリアライズ
 */
void ONEngine::to_json(nlohmann::json& _j, const AudioSource& _a) {
	_j = nlohmann::json{
		{ "type", "AudioSource" },
		{ "enable", _a.enable },
		{ "volume", _a.GetVolume() },
		{ "pitch", _a.GetPitch() },
		{ "path", _a.GetAudioPath() }
	};
}