#include "AudioSource.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/Asset/Assets/AudioClip/AudioClip.h"
#include "Engine/Core/Utility/Math/Mathf.h"
#include "Engine/Core/Utility/Tools/Log.h"

/// editor
#include "Engine/Editor/Math/ImGuiMath.h"

using namespace ONEngine;

AudioSource::AudioSource()
	: volume_(1.0f),
	pitch_(1.0f),
	state_(0),
	isPlayingRequest_(false){
}

AudioSource::~AudioSource() {}

void AudioSource::Play() {
	isPlayingRequest_ = true;
}

void AudioSource::PlayOneShot(float _volume, float _pitch, const std::string& _path) {
	oneShotAudioRequests_.push_back({ _path, _volume, _pitch });
}

void AudioSource::AddSourceVoice(IXAudio2SourceVoice* _sourceVoice) {
	sourceVoices_.push_back(_sourceVoice);
}

void AudioSource::SetVolume(float _volume) {
	volume_ = _volume;
}

void AudioSource::SetPitch(float _pitch) {
	pitch_ = _pitch;
}

void AudioSource::SetAudioPath(const std::string& _path) {
	path_ = _path;
}

void AudioSource::SetAudioClip(AudioClip* _clip) {
	pAudioClip_ = _clip;
}

float AudioSource::GetVolume() const {
	return volume_;
}

float AudioSource::GetPitch() const {
	return pitch_;
}

const std::string& AudioSource::GetAudioPath() const {
	return path_;
}

AudioClip* AudioSource::GetAudioClip() const {
	return pAudioClip_;
}

int AudioSource::GetState() const {
	return state_;
}


/// 

void COMP_DEBUG::AudioSourceDebug(AudioSource* _as) {
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
				const char* droppedPath = static_cast<const char*>(payload->Data);
				const std::string&& path = std::string(droppedPath);
				const std::string&& extension = Mathf::FileExtension(path);

				/// Audioのパスが有効な形式か確認
				if (extension == ".mp3" ||
					extension == ".wav") {
					_as->SetAudioPath(path);

					Console::Log(std::format("Audio path set to: {}", path));
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
	ImGui::Text("State: %s", state == AudioState_Playing ? "Playing" : "Stopped");



}

void MONO_INTERNAL_METHOD::InternalGetParams(uint64_t _nativeHandle, float* _volume, float* _pitch) {
	AudioSource* audioSource = reinterpret_cast<AudioSource*>(_nativeHandle);
	if (!audioSource) {
		Console::LogError("AudioSource pointer is null");
		return;
	}

	*_volume = audioSource->GetVolume();
	*_pitch = audioSource->GetPitch();

}

void MONO_INTERNAL_METHOD::InternalSetParams(uint64_t _nativeHandle, float _volume, float _pitch) {
	AudioSource* audioSource = reinterpret_cast<AudioSource*>(_nativeHandle);
	if (!audioSource) {
		Console::LogError("AudioSource pointer is null");
		return;
	}

	audioSource->SetVolume(_volume);
	audioSource->SetPitch(_pitch);
}

void MONO_INTERNAL_METHOD::InternalPlayOneShot(uint64_t _nativeHandle, float _volume, float _pitch, MonoString* _path) {
	/// 音の再生
	AudioSource* audioSource = reinterpret_cast<AudioSource*>(_nativeHandle);
	if (!audioSource) {
		Console::LogError("AudioSource pointer is null");
		return;
	}

	/// pathの変換
	char* path = mono_string_to_utf8(_path);

	audioSource->PlayOneShot(_volume, _pitch, std::string(path));

	mono_free(path);
}


/// json serialize
void ONEngine::from_json(const nlohmann::json& _j, AudioSource& _a) {
	_a.enable = _j.value("enable", 1);
	_a.SetVolume(_j.value("volume", 1.0f));
	_a.SetPitch(_j.value("pitch", 1.0f));
	_a.SetAudioPath(_j.value("path", std::string("")));
}

void ONEngine::to_json(nlohmann::json& _j, const AudioSource& _a) {
	_j = nlohmann::json{
		{ "type", "AudioSource" },
		{ "enable", _a.enable },
		{ "volume", _a.GetVolume() },
		{ "pitch", _a.GetPitch() },
		{ "path", _a.GetAudioPath() }
	};
}