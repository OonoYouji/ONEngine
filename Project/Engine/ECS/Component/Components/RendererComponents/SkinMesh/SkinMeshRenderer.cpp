#include "SkinMeshRenderer.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Core/Utility/Tools/StringHash.h"
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"
#include "Engine/ECS/EntityComponentSystem/ECSGroup.h"
#include "Engine/ECS/Component/Array/ComponentArray.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Animator/Animator.h"

/// editor
#include "Engine/Editor/Math/ImGuiMath.h"
#include "Engine/Editor/Math/AssetPayload.h"

using namespace ONEngine;

SkinMeshRenderer::SkinMeshRenderer() {
	SetMeshPath("./Packages/Models/Human/walk.gltf");
	SetTexturePath("./Packages/Textures/white.png");

	isPlaying_ = true;
	animationTime_ = 0.0f;
	duration_ = 0.0f;
	animationScale_ = 1.0f;
	debugJointSize_ = 1.0f;
	debugRectSize_ = 1.0f;


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

void SkinMeshRenderer::SetDebugJointSize(float _size) {
	debugJointSize_ = _size;
}

void SkinMeshRenderer::SetDebugRectSize(float _size) {
	debugRectSize_ = _size;
}

void SkinMeshRenderer::SetNodeAnimationMap(const std::unordered_map<uint32_t, NodeAnimation>& _map) {
	nodeAnimationMap_ = _map;
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

float SkinMeshRenderer::GetDebugJointSize() const {
	return debugJointSize_;
}

float SkinMeshRenderer::GetDebugRectSize() const {
	return debugRectSize_;
}

const Skeleton& SkinMeshRenderer::GetSkeleton() const {
	return skeleton_;
}

const Vector4& SkinMeshRenderer::GetColor() const {
	return color_;
}




void ComponentDebug::SkinMeshRendererDebug(SkinMeshRenderer* _smr, Asset::AssetCollection* _assetCollection) {
	if (_smr == nullptr) {
		return;
	}

	/// param get
	std::string meshPath = _smr->GetMeshPath();
	std::string texturePath = _smr->GetTexturePath();

	bool isPlaying = _smr->GetIsPlaying();
	float animationTime = _smr->GetAnimationTime();
	float duration = _smr->GetDuration();
	float jointSize = _smr->GetDebugJointSize();
	float rectSize = _smr->GetDebugRectSize();
	Vector4 color = _smr->GetColor();

	// --- 1. 基本設定 ---
	if (ImGui::CollapsingHeader("Base Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (ImGui::Checkbox("is playing", &isPlaying)) {
			_smr->SetIsPlaying(isPlaying);
		}

		if (Editor::ImGuiColorEdit("color", &color)) {
			_smr->SetColor(color);
		}

		if (ImGui::DragFloat("joint size", &jointSize, 0.01f, 0.0f, 100.0f)) {
			_smr->SetDebugJointSize(jointSize);
		}
		if (ImGui::DragFloat("rect size", &rectSize, 0.01f, 0.0f, 100.0f)) {
			_smr->SetDebugRectSize(rectSize);
		}
	}

	// --- 2. アニメーションデバッグ (最重要) ---
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "--- ANIMATION DEBUG ---");

	// モデル情報の取得
	Asset::Model* model = _assetCollection->GetModel(meshPath);
	if (!model) {
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "MODEL NOT FOUND: %s", meshPath.c_str());
		ImGui::TextDisabled("(Check if path matches AssetCollection registration)");
	} else {
		const auto& clips = model->GetAnimationClips();
		ImGui::Text("Model: Found");
		ImGui::Text("Clips: %zu available", clips.size());

		// Animatorコンポーネントの検索
		auto* owner = _smr->GetOwner();
		Animator* animator = nullptr;
		if (owner) {
			ImGui::Text("Entity: %s (ID: %u)", owner->GetName().c_str(), owner->GetId());
			animator = owner->GetComponent<Animator>();
		} else {
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "Animator: Owner is NULL");
		}

		if (animator) {
			if (animator->enable) {
				// 現在のアニメーション特定
				std::string mainClipName = "None";
				float maxWeight = -1.0f;
				for (uint32_t i = 0; i < MAX_ANIMATION_LAYERS; ++i) {
					if (animator->layers[i].weight <= 0.0f) continue;
					for (uint32_t j = 0; j < MAX_ANIMATION_STATES_PER_LAYER; ++j) {
						if (animator->layers[i].states[j].weight > maxWeight) {
							maxWeight = animator->layers[i].states[j].weight;
							auto it = clips.find(animator->layers[i].states[j].clipId);
							if (it != clips.end()) mainClipName = it->second.name;
						}
					}
				}

				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Playing Animation: ");
				ImGui::SameLine();
				ImGui::Text("%s", mainClipName.c_str());
				ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "[Driven by Animator]");

				// --- デフォルトクリップの選択 (改善版) ---
				ImGui::Spacing();
				std::string defaultClipName = "None (0)";
				auto itDefault = clips.find(animator->GetDefaultClip());
				if (itDefault != clips.end()) {
					defaultClipName = itDefault->second.name;
				}

				if (ImGui::BeginCombo("Default Clip", defaultClipName.c_str())) {
					if (ImGui::Selectable("None", animator->GetDefaultClip() == 0)) {
						animator->SetDefaultClip(0);
					}
					for (const auto& [hash, clip] : clips) {
						bool isSelected = (animator->GetDefaultClip() == hash);
						if (ImGui::Selectable(clip.name.c_str(), isSelected)) {
							animator->SetDefaultClip(hash);
							animator->Play(hash); // 即座に再生を開始して確認できるようにする
							Console::Log(std::format("Default Clip set and playing: {} (hash: {})", clip.name, hash));
						}
						if (isSelected) ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				if (ImGui::IsItemHovered()) {
					ImGui::SetTooltip("Select the animation to play automatically on start.");
				}

				if (ImGui::TreeNode("Clip Preview / Test Play")) {
					for (const auto& [hash, clip] : clips) {
						bool isCurrent = (mainClipName == clip.name);

						if (ImGui::Selectable(clip.name.c_str(), isCurrent)) {
							animator->Play(hash);
							Console::Log(std::format("Preview Clip: {}", clip.name));
						}
					}
					ImGui::TreePop();
				}
			} else {
				ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Animator is DISABLED.");
			}
		} else {
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "No Animator component on this entity.");
		}
	}

	ImGui::Separator();

	/// meshの変更
	ImGui::Text("mesh path");
	Editor::ImMathf::InputText("##mesh", &meshPath, ImGuiInputTextFlags_ReadOnly);
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetData")) {

			/// ペイロードが存在する場合
			if (payload->Data) {
				Editor::AssetPayload* assetPayload = *static_cast<Editor::AssetPayload**>(payload->Data);
				const std::string path = assetPayload->filePath;
				Asset::AssetType type = Asset::GetAssetTypeFromExtension(FileSystem::FileExtension(path));

				/// メッシュのパスが有効な形式か確認
				if (type == Asset::AssetType::Mesh) {
					_smr->SetMeshPath(path);

					Console::Log(std::format("Mesh path set to: {}", path));
				} else {
					Console::LogError("Invalid mesh format. Please use .obj or .gltf.");
				}
			}
		}
		ImGui::EndDragDropTarget();
	}


	/// textureの変更
	ImGui::Text("texture path");

	/// テクスチャのプレビュー表示
	if (Asset::Texture* tex = _assetCollection->GetTexture(texturePath)) {
		Vector2 aspectRatio = tex->GetTextureSize();
		aspectRatio /= (std::max)(aspectRatio.x, aspectRatio.y);

		ImTextureID texId = reinterpret_cast<ImTextureID>(tex->GetSRVGPUHandle().ptr);
		ImGui::Image(texId, ImVec2(64.0f * aspectRatio.x, 64.0f * aspectRatio.y));
	} else {
		/// テクスチャがない場合はドラッグドロップ領域を表示する
		ImVec2 size = ImVec2(64, 64);
		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		ImGui::InvisibleButton("DropArea", size);

		ImU32 imColor = IM_COL32(100, 100, 255, 100);
		drawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), imColor, 4.0f);
		drawList->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(255, 255, 255, 200), 4.0f, 0, 2.0f);
	}

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetData")) {

			/// ペイロードが存在する場合
			if (payload->Data) {
				Editor::AssetPayload* assetPayload = *static_cast<Editor::AssetPayload**>(payload->Data);
				const std::string path = assetPayload->filePath;
				Asset::AssetType type = Asset::GetAssetTypeFromExtension(FileSystem::FileExtension(path));

				/// テクスチャのパスが有効な形式か確認
				if (type == Asset::AssetType::Texture) {
					_smr->SetTexturePath(path);

					Console::Log(std::format("Texture path set to: {}", path));
				} else {
					Console::LogError("Invalid texture format. Please use .png, .jpg, or .jpeg.");
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
	if (_j.contains("debugJointSize")) {
		_smr.SetDebugJointSize(_j.at("debugJointSize").get<float>());
	}
	if (_j.contains("debugRectSize")) {
		_smr.SetDebugRectSize(_j.at("debugRectSize").get<float>());
	}
}

void ONEngine::to_json(nlohmann::json& _j, const SkinMeshRenderer& _smr) {
	_j = nlohmann::json{
		{ "type", "SkinMeshRenderer" },
		{ "enable", _smr.enable },
		{ "meshPath", _smr.GetMeshPath() },
		{ "texturePath", _smr.GetTexturePath() },
		{ "isPlaying", _smr.GetIsPlaying() },
		{ "animationScale", _smr.GetAnimationScale() },
		{ "debugJointSize", _smr.GetDebugJointSize() },
		{ "debugRectSize", _smr.GetDebugRectSize() }
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
	uint32_t jointNameHash = StringHash::Get(jointName);
	mono_free(jointNameChars);

	/// ジョイントのトランスフォームを取得
	if (smr->GetSkeleton().jointMap.contains(jointNameHash) == false) {
		Console::LogError(std::format("SkinMeshRenderer::InternalGetJointTransform: Joint '{}' not found in skeleton.", jointName));
		*_outScale = Vector3::One; ///< ジョイントが見つからない場合はゼロベクトルを設定
		*_outRotation = Quaternion::kIdentity; ///< ジョイントが見つからない場合は単位クォータニオンを設定
		*_outPosition = Vector3::Zero; ///< ジョイントが見つからない場合はゼロベクトルを設定

		return; ///< ジョイントが見つからない場合は何もしない
	}

	int32_t jointIndex = smr->GetSkeleton().jointMap.at(jointNameHash);
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
