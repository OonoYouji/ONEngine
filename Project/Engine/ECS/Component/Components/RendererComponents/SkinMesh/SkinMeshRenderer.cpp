#include "SkinMeshRenderer.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/Core/Utility/Utility.h"


SkinMeshRenderer::SkinMeshRenderer() {
	SetMeshPath("./Packages/Models/Human/walk.gltf");
	SetTexturePath("./Packages/Textures/white.png");
	animationTime_ = 0.0f;
	duration_ = 0.0f;
	animationScale_ = 1.0f;
	color_ = Color::kWhite;
	isChangingMesh_ = true; /// 初期化時にメッシュを変更するフラグを立てる
}

void SkinMeshRenderer::SetMeshPath(const std::string& _path) {
	isChangingMesh_ = true;
	meshPath_ = _path;
}

void SkinMeshRenderer::SetTexturePath(const std::string& _path) {
	texturePath_ = _path;
}

void SkinMeshRenderer::SetColor(const Vector4& _color) {
	color_ = _color;
}

void SkinMeshRenderer::SetIsPlaying(bool _isPlaying) {
	isPlaying_ = _isPlaying;
}

void SkinMeshRenderer::SetAnimationTime(float _time) {
	animationTime_ = _time;
}

void SkinMeshRenderer::SetDuration(float _duration) {
	duration_ = _duration;
}

void SkinMeshRenderer::SetAnimationScale(float _scale) {
	animationScale_ = _scale;
}

const std::string& SkinMeshRenderer::GetMeshPath() const {
	return meshPath_;
}

const std::string& SkinMeshRenderer::GetTexturePath() const {
	return texturePath_;
}

bool SkinMeshRenderer::GetIsPlaying() const {
	return isPlaying_;
}

float SkinMeshRenderer::GetAnimationTime() const {
	return animationTime_;
}

float SkinMeshRenderer::GetDuration() const {
	return duration_;
}

float SkinMeshRenderer::GetAnimationScale() const {
	return animationScale_;
}

const Vector4& SkinMeshRenderer::GetColor() const {
	return color_;
}




void COMP_DEBUG::SkinMeshRendererDebug(SkinMeshRenderer* _smr) {
	if (_smr == nullptr) {
		return;
	}

	/// param get
	std::string meshPath = _smr->GetMeshPath();
	std::string texturePath = _smr->GetTexturePath();

	bool isPlaying = _smr->GetIsPlaying();
	float animationTime = _smr->GetAnimationTime();
	float duration = _smr->GetDuration();


	if (ImGui::Checkbox("is playing", &isPlaying)) {
		_smr->SetIsPlaying(isPlaying);
	}

	/// edit
	if (ImGui::DragFloat("animation time", &animationTime, 0.01f, 0.0f, duration)) {
		_smr->SetAnimationTime(animationTime);
	}

	if (ImGui::DragFloat("duration", &duration, 0.01f, 0.0f, 0.0f, "%.3f", ImGuiSliderFlags_None)) {
		_smr->SetDuration(duration);
	}


	ImGui::Spacing();


	/// meshの変更
	ImGui::Text("mesh path");
	ImGui::InputText("##mesh", meshPath.data(), meshPath.capacity(), ImGuiInputTextFlags_ReadOnly);
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetData")) {

			/// ペイロードが存在する場合
			if (payload->Data) {
				const char* droppedPath = static_cast<const char*>(payload->Data);
				std::string path = std::string(droppedPath);

				/// メッシュのパスが有効な形式か確認
				if (path.find(".gltf") != std::string::npos) {
					_smr->SetMeshPath(path);

					Console::Log(std::format("Mesh path set to: {}", path));
				} else {
					Console::Log("Invalid mesh format. Please use .gltf.");
				}
			}
		}
		ImGui::EndDragDropTarget();
	}


	/// textureの変更
	ImGui::Text("texture path");
	ImGui::InputText("##texture", texturePath.data(), texturePath.capacity(), ImGuiInputTextFlags_ReadOnly);
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetData")) {

			/// ペイロードが存在する場合
			if (payload->Data) {
				const char* droppedPath = static_cast<const char*>(payload->Data);
				std::string path = std::string(droppedPath);

				/// テクスチャのパスが有効な形式か確認
				if (path.find(".png") != std::string::npos
					|| path.find(".jpg") != std::string::npos
					|| path.find(".jpeg") != std::string::npos) {
					_smr->SetTexturePath(path);

					Console::Log(std::format("Texture path set to: {}", path));
				} else {
					Console::Log("Invalid texture format. Please use .png, .jpg, or .jpeg.");
				}
			}
		}

		ImGui::EndDragDropTarget();
	}

}

void from_json(const nlohmann::json& _j, SkinMeshRenderer& _smr) {
	_smr.enable = _j.at("enable").get<int>();
	_smr.SetMeshPath(_j.at("meshPath").get<std::string>());
	_smr.SetTexturePath(_j.at("texturePath").get<std::string>());
}

void to_json(nlohmann::json& _j, const SkinMeshRenderer& _smr) {
	_j = nlohmann::json{
		{ "type", "SkinMeshRenderer" },
		{ "enable", _smr.enable },
		{ "meshPath", _smr.GetMeshPath() },
		{ "texturePath", _smr.GetTexturePath() },
		//{ "color", _smr.GetColor() },
	};
}


/// ====================================================
/// internal methods
/// ====================================================

SkinMeshRenderer* GetSkinMeshRenderer(uint64_t _nativeHandle) {
	return reinterpret_cast<SkinMeshRenderer*>(_nativeHandle);
}

MonoString* InternalGetMeshPath(uint64_t _nativeHandle) {
	SkinMeshRenderer* smr = GetSkinMeshRenderer(_nativeHandle);

	const std::string& meshPath = smr->GetMeshPath();
	MonoString* monoMeshPath = mono_string_new(mono_domain_get(), meshPath.c_str());
	return monoMeshPath;
}

void InternalSetMeshPath(uint64_t _nativeHandle, MonoString* _path) {
	SkinMeshRenderer* smr = GetSkinMeshRenderer(_nativeHandle);
	/// MonoStringからstd::stringに変換
	char* pathChars = mono_string_to_utf8(_path);
	std::string path(pathChars);
	mono_free(pathChars);
	smr->SetMeshPath(path);
}

MonoString* InternalGetTexturePath(uint64_t _nativeHandle) {
	SkinMeshRenderer* smr = GetSkinMeshRenderer(_nativeHandle);

	const std::string& texturePath = smr->GetTexturePath();
	MonoString* monoTexturePath = mono_string_new(mono_domain_get(), texturePath.c_str());
	return monoTexturePath;
}

void InternalSetTexturePath(uint64_t _nativeHandle, MonoString* _path) {
	SkinMeshRenderer* smr = GetSkinMeshRenderer(_nativeHandle);
	/// MonoStringからstd::stringに変換
	char* pathChars = mono_string_to_utf8(_path);
	std::string path(pathChars);
	mono_free(pathChars);
	smr->SetTexturePath(path);
}

bool InternalGetIsPlaying(uint64_t _nativeHandle) {
	SkinMeshRenderer* smr = GetSkinMeshRenderer(_nativeHandle);
	return smr->GetIsPlaying();
}

void InternalSetIsPlaying(uint64_t _nativeHandle, bool _isPlaying) {
	SkinMeshRenderer* smr = GetSkinMeshRenderer(_nativeHandle);
	smr->SetIsPlaying(_isPlaying);
}

float InternalGetAnimationTime(uint64_t _nativeHandle) {
	SkinMeshRenderer* smr = GetSkinMeshRenderer(_nativeHandle);
	return smr->GetAnimationTime();
}

void InternalSetAnimationTime(uint64_t _nativeHandle, float _time) {
	SkinMeshRenderer* smr = GetSkinMeshRenderer(_nativeHandle);
	smr->SetAnimationTime(_time);
}

float InternalGetAnimationScale(uint64_t _nativeHandle) {
	SkinMeshRenderer* smr = GetSkinMeshRenderer(_nativeHandle);
	return smr->GetAnimationScale();
}

void InternalSetAnimationScale(uint64_t _nativeHandle, float _scale) {
	SkinMeshRenderer* smr = GetSkinMeshRenderer(_nativeHandle);
	smr->SetAnimationScale(_scale);
}
