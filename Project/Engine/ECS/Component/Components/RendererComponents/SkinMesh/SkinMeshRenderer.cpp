#include "SkinMeshRenderer.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/Core/Utility/Utility.h"

/// editor
#include "Engine/Editor/Math/ImGuiMath.h"

using namespace ONEngine;

SkinMeshRenderer::SkinMeshRenderer() {
	SetMeshPath("./Packages/Models/Human/walk.gltf");
	SetTexturePath("./Packages/Textures/white.png");

	isPlaying_ = true;
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

const Skeleton& SkinMeshRenderer::GetSkeleton() const {
	return skeleton_;
}

const Vector4& SkinMeshRenderer::GetColor() const {
	return color_;
}




void ComponentDebug::SkinMeshRendererDebug(SkinMeshRenderer* _smr) {
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


	ImGui::Indent();
	
	if (ImGui::CollapsingHeader("joints")) {

		for (const auto& jointIndex : _smr->GetSkeleton().jointMap) {
			/// ジョイント名を表示
			const Joint& joint = _smr->GetSkeleton().joints[jointIndex.second];

			std::string pointerName = joint.name;
			Editor::ImMathf::InputText(std::string("##" + pointerName).c_str(), &pointerName, ImGuiInputTextFlags_ReadOnly);

			/// 最後に次のジョイントの間隔を空ける
			ImGui::Spacing();
		}
	}

	ImGui::Unindent();


}

void ONEngine::from_json(const nlohmann::json& _j, SkinMeshRenderer& _smr) {
	_smr.enable = _j.at("enable").get<int>();
	_smr.SetMeshPath(_j.at("meshPath").get<std::string>());
	_smr.SetTexturePath(_j.at("texturePath").get<std::string>());
}

void ONEngine::to_json(nlohmann::json& _j, const SkinMeshRenderer& _smr) {
	_j = nlohmann::json{
		{ "type", "SkinMeshRenderer" },
		{ "enable", _smr.enable },
		{ "meshPath", _smr.GetMeshPath() },
		{ "texturePath", _smr.GetTexturePath() },
		{ "isPlaying", _smr.GetIsPlaying() },
		{ "animationScale", _smr.GetAnimationScale() }
	};
}


/// ====================================================
/// internal methods
/// ====================================================

SkinMeshRenderer* ONEngine::GetSkinMeshRenderer(uint64_t _nativeHandle) {
	return reinterpret_cast<SkinMeshRenderer*>(_nativeHandle);
}

MonoString* ONEngine::InternalGetMeshPath(uint64_t _nativeHandle) {
	SkinMeshRenderer* smr = GetSkinMeshRenderer(_nativeHandle);

	const std::string& meshPath = smr->GetMeshPath();
	MonoString* monoMeshPath = mono_string_new(mono_domain_get(), meshPath.c_str());
	return monoMeshPath;
}

void ONEngine::InternalSetMeshPath(uint64_t _nativeHandle, MonoString* _path) {
	SkinMeshRenderer* smr = GetSkinMeshRenderer(_nativeHandle);
	/// MonoStringからstd::stringに変換
	char* pathChars = mono_string_to_utf8(_path);
	std::string path(pathChars);
	mono_free(pathChars);
	smr->SetMeshPath(path);
}

MonoString* ONEngine::InternalGetTexturePath(uint64_t _nativeHandle) {
	SkinMeshRenderer* smr = GetSkinMeshRenderer(_nativeHandle);

	const std::string& texturePath = smr->GetTexturePath();
	MonoString* monoTexturePath = mono_string_new(mono_domain_get(), texturePath.c_str());
	return monoTexturePath;
}

void ONEngine::InternalSetTexturePath(uint64_t _nativeHandle, MonoString* _path) {
	SkinMeshRenderer* smr = GetSkinMeshRenderer(_nativeHandle);
	/// MonoStringからstd::stringに変換
	char* pathChars = mono_string_to_utf8(_path);
	std::string path(pathChars);
	mono_free(pathChars);
	smr->SetTexturePath(path);
}

bool ONEngine::InternalGetIsPlaying(uint64_t _nativeHandle) {
	SkinMeshRenderer* smr = GetSkinMeshRenderer(_nativeHandle);
	return smr->GetIsPlaying();
}

void ONEngine::InternalSetIsPlaying(uint64_t _nativeHandle, bool _isPlaying) {
	SkinMeshRenderer* smr = GetSkinMeshRenderer(_nativeHandle);
	smr->SetIsPlaying(_isPlaying);
}

float ONEngine::InternalGetAnimationTime(uint64_t _nativeHandle) {
	SkinMeshRenderer* smr = GetSkinMeshRenderer(_nativeHandle);
	return smr->GetAnimationTime();
}

void ONEngine::InternalSetAnimationTime(uint64_t _nativeHandle, float _time) {
	SkinMeshRenderer* smr = GetSkinMeshRenderer(_nativeHandle);
	smr->SetAnimationTime(_time);
}

float ONEngine::InternalGetAnimationScale(uint64_t _nativeHandle) {
	SkinMeshRenderer* smr = GetSkinMeshRenderer(_nativeHandle);
	return smr->GetAnimationScale();
}

void ONEngine::InternalSetAnimationScale(uint64_t _nativeHandle, float _scale) {
	SkinMeshRenderer* smr = GetSkinMeshRenderer(_nativeHandle);
	smr->SetAnimationScale(_scale);
}

void ONEngine::InternalGetJointTransform(uint64_t _nativeHandle, MonoString* _jointName, Vector3* _outScale, Quaternion* _outRotation, Vector3* _outPosition) {
	SkinMeshRenderer* smr = GetSkinMeshRenderer(_nativeHandle);
	if (!smr) {
		Console::LogError("SkinMeshRenderer::InternalGetJointTransform: SkinMeshRenderer is null.");
		return; ///< SkinMeshRendererがnullの場合は何もしない
	}

	/// MonoStringからstd::stringに変換
	char* jointNameChars = mono_string_to_utf8(_jointName);
	std::string jointName(jointNameChars);
	mono_free(jointNameChars);

	/// ジョイントのトランスフォームを取得
	if (smr->GetSkeleton().jointMap.contains(jointName) == false) {
		Console::LogError(std::format("SkinMeshRenderer::InternalGetJointTransform: Joint '{}' not found in skeleton.", jointName));
		*_outScale = Vector3::kOne; ///< ジョイントが見つからない場合はゼロベクトルを設定
		*_outRotation = Quaternion::kIdentity; ///< ジョイントが見つからない場合は単位クォータニオンを設定
		*_outPosition = Vector3::kZero; ///< ジョイントが見つからない場合はゼロベクトルを設定

		return; ///< ジョイントが見つからない場合は何もしない
	}

	int32_t jointIndex = smr->GetSkeleton().jointMap.at(jointName);
	const Matrix4x4& matWorld = smr->GetSkeleton().joints[jointIndex].matWorld;


	/// 出力パラメータに値を設定
	if (_outScale) {
		*_outScale = matWorld.ExtractScale();
	}
	if (_outRotation) {
		*_outRotation = matWorld.ExtractRotation();
	}
	if (_outPosition) {
		*_outPosition = matWorld.ExtractTranslation();
	}
}
