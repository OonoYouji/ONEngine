#include "ImGuiMath.h"

#define NOMINMAX

/// std
#include <numbers>
#include <format>
#include <variant>
#include <algorithm>
#include <cstdio> 
#include <cmath> 

/// external
#include <imgui_internal.h> // PushMultiItemsWidths に必要
#include <Externals/imgui/dialog/ImGuiFileDialog.h>

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Light/Light.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Audio/AudioSource.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Effect/Effect.h"
#include "Engine/ECS/Component/Components/RendererComponents/Mesh/MeshRenderer.h"
#include "Engine/ECS/Component/Components/RendererComponents/Mesh/CustomMeshRenderer.h"

/// editor
#include "Engine/Editor/Manager/EditCommand.h"
#include "Engine/Editor/Commands/ImGuiCommand/ImGuiCommand.h" 
#include "Engine/Editor/Math/AssetPayload.h"

using namespace Editor;

namespace {

float rotateSpeed = std::numbers::pi_v<float> / 100.0f;

std::string variableName = "";

}	/// unnamed namespace

ImVec4 ImMathf::ToImVec4(const ONEngine::Vector4& _vec) {
	return ImVec4(_vec.x, _vec.y, _vec.z, _vec.w);
}

ImVec2 ImMathf::ToImVec2(const ONEngine::Vector2& _vec) {
	return ImVec2(_vec.x, _vec.y);
}

bool ImMathf::ColorEdit(const char* _label, ONEngine::Vector4* _color, ImGuiColorEditFlags _flags) {
	if(!_color) {
		return false;
	}

	if(ImGui::ColorEdit4(_label, &_color->x, _flags)) {
		return true; // 色が変更された
	}

	return false;
}

bool ImMathf::InputText(const char* _label, std::string* _text, ImGuiInputTextFlags _flags) {
	if(!_text) {
		return false; // nullptr check
	}

	_flags |= ImGuiInputTextFlags_CallbackResize;

	struct CallbackUserData {
		std::string* text;
	};

	auto callback = [](ImGuiInputTextCallbackData* data) -> int {
		if(data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
			auto* user = static_cast<CallbackUserData*>(data->UserData);

			/// stringのサイズを変更
			user->text->resize(data->BufTextLen);
			data->Buf = user->text->data();
		}
		return 0;
	};

	CallbackUserData userData = { _text };
	return ImGui::InputText(_label, _text->data(), _text->capacity(), _flags,
							callback, &userData
	);
}

bool ImMathf::MaterialEdit(const char* _label, ONEngine::GPUMaterial* _material, ONEngine::AssetCollection* _assetCollection) {
	/// nullptr check
	if(!_material) {
		return false;
	}

	bool isEdit = false;
	if(ImGui::CollapsingHeader(_label)) {
		if(ImGuiColorEdit("BaseColor", &_material->baseColor)) {
			isEdit = true;
		}

		if(UVTransformEdit("UVTransform", &_material->uvTransform)) {
			isEdit = true;
		}


		/// ---------------------------------------------------
		/// ポストエフェクトフラグの編集
		/// ---------------------------------------------------
		if(ImGui::CollapsingHeader("PostEffectFlags")) {
			/// ポストエフェクトのフラグ
			if(ImGui::CheckboxFlags("Lighting", &_material->postEffectFlags, PostEffectFlags_Lighting)) {
				isEdit = true;
			}

			if(ImGui::CheckboxFlags("Grayscale", &_material->postEffectFlags, PostEffectFlags_Grayscale)) {
				isEdit = true;
			}

			if(ImGui::CheckboxFlags("EnvironmentReflection", &_material->postEffectFlags, PostEffectFlags_EnvironmentReflection)) {
				isEdit = true;
			}

			if(ImGui::CheckboxFlags("Shadow", &_material->postEffectFlags, PostEffectFlags_Shadow)) {
				isEdit = true;
			}

		}


		/// ---------------------------------------------------
		/// テクスチャの編集
		/// ---------------------------------------------------
		if(ImGui::CollapsingHeader("Texture")) {

			/// textureの変更
			const std::string& texturePath = _assetCollection->GetTexturePath(_material->baseTextureId);
			ImMathf::InputText("Base Texture", const_cast<std::string*>(&texturePath), ImGuiInputTextFlags_ReadOnly);
			if(ImGui::BeginDragDropTarget()) {
				if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetData")) {
					if(payload->Data) {
						AssetPayload* assetPayload = *static_cast<AssetPayload**>(payload->Data);
						std::string path = assetPayload->filePath;

						ONEngine::AssetType type = ONEngine::GetAssetTypeFromExtension(ONEngine::FileSystem::FileExtension(path));
						if(type == ONEngine::AssetType::Texture) {
							size_t droppedTextureIndex = _assetCollection->GetTextureIndex(path);
							_material->baseTextureId = static_cast<int32_t>(droppedTextureIndex);
							isEdit = true;
						}
					}
				}
				ImGui::EndDragDropTarget();
			}

			/// texture idが有効値じゃなければ無視
			if(_material->baseTextureId >= 0) {
				const ONEngine::Texture* baseTexture = _assetCollection->GetTexture(_assetCollection->GetTexturePath(_material->baseTextureId));
				if(baseTexture) {
					ImTextureID textureId = reinterpret_cast<ImTextureID>(baseTexture->GetSRVGPUHandle().ptr);
					ImGui::Image(textureId, ImVec2(100, 100), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 0));
				}
			}


			ImGui::Spacing();


			/// 法線テクスチャの変更
			const std::string& normalTexturePath = _assetCollection->GetTexturePath(_material->normalTextureId);
			ImMathf::InputText("Normal Texture", const_cast<std::string*>(&normalTexturePath), ImGuiInputTextFlags_ReadOnly);
			if(ImGui::BeginDragDropTarget()) {
				if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetData")) {
					if(payload->Data) {
						const char* droppedPath = static_cast<const char*>(payload->Data);
						std::string path = std::string(droppedPath);
						if(path.find(".png") != std::string::npos || path.find(".jpg") != std::string::npos) {
							size_t droppedTextureIndex = _assetCollection->GetTextureIndex(path);
							_material->normalTextureId = static_cast<int32_t>(droppedTextureIndex);
							isEdit = true;
						}
					}
				}
				ImGui::EndDragDropTarget();
			}


			/// normal texture idが有効値じゃなければ無視
			if(_material->normalTextureId >= 0) {
				const ONEngine::Texture* normalTexture = _assetCollection->GetTexture(_assetCollection->GetTexturePath(_material->normalTextureId));
				if(normalTexture) {
					ImTextureID textureId = reinterpret_cast<ImTextureID>(normalTexture->GetSRVGPUHandle().ptr);
					ImGui::Image(textureId, ImVec2(100, 100), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 0));
				}
			}


		}


	}


	return isEdit;
}

bool ImMathf::UVTransformEdit(const char* _label, ONEngine::UVTransform* _uvTransform) {
	/// nullptr check
	if(!_uvTransform) {
		return false;
	}

	bool isEdit = false;

	ImGui::PushID(2);

	if(ImGui::CollapsingHeader(_label)) {
		/// UVのオフセット
		if(ImGui::DragFloat2("offset", &_uvTransform->position.x, 0.01f)) {
			isEdit = true;
		}
		/// UVのスケール
		if(ImGui::DragFloat2("scale", &_uvTransform->scale.x, 0.01f, 0.0f, FLT_MAX)) {
			isEdit = true;
		}
		/// UVの回転
		if(ImGui::DragFloat("rotate", &_uvTransform->rotate, 0.01f, -std::numbers::pi_v<float>, std::numbers::pi_v<float>)) {
			isEdit = true;
		}

	} // if ImGui::CollapsingHeader

	ImGui::PopID();

	return isEdit;
}

ImVec2 ImMathf::CalculateAspectFitSize(const ONEngine::Vector2& _textureSize, float _maxSize) {
	// アスペクト比を計算
	float aspectRatio = _textureSize.x / _textureSize.y;

	// 最大サイズに基づいて幅と高さを計算
	float width = _maxSize;
	float height = _maxSize;

	if(aspectRatio > 1.0f) {
		// 横長の場合
		height = _maxSize / aspectRatio;
	} else {
		// 縦長または正方形の場合
		width = _maxSize * aspectRatio;
	}

	return ImVec2(width, height);
}

ImVec2 ImMathf::CalculateAspectFitSize(const ONEngine::Vector2& _textureSize, const ImVec2& _maxSize) {
	// アスペクト比を計算
	float aspectRatio = _textureSize.x / _textureSize.y;

	// 最大サイズに基づいて幅と高さを計算
	float width = _maxSize.x;
	float height = _maxSize.y;

	if(aspectRatio > (_maxSize.x / _maxSize.y)) {
		// 横長の場合、幅を最大にして高さを調整
		height = _maxSize.x / aspectRatio;
	} else {
		// 縦長または正方形の場合、高さを最大にして幅を調整
		width = _maxSize.y * aspectRatio;
	}

	return ImVec2(width, height);
}


bool Editor::ImGuiInputText(const char* _label, std::string* _text, ImGuiInputTextFlags _flags) {
	if(!_text) {
		return false; // nullptr check
	}

	_flags |= ImGuiInputTextFlags_CallbackResize;

	struct CallbackUserData {
		std::string* text;
	};

	auto callback = [](ImGuiInputTextCallbackData* data) -> int {
		if(data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
			auto* user = static_cast<CallbackUserData*>(data->UserData);
			user->text->resize(data->BufTextLen);   // ← 入力が減っても size が追従する！
			data->Buf = user->text->data();
		}
		return 0;
	};

	CallbackUserData userData = { _text };
	return ImGui::InputText(_label, _text->data(), _text->capacity(), _flags,
							callback, &userData
	);
}

void Editor::ImGuiInputTextReadOnly(const char* _label, const std::string& _text) {
	char buffer[256];
	strncpy_s(buffer, _text.c_str(), sizeof(buffer));
	buffer[sizeof(buffer) - 1] = '\0';
	ImGui::InputText(_label, buffer, sizeof(buffer), ImGuiInputTextFlags_ReadOnly);
}

bool Editor::ImGuiColorEdit(const char* _label, ONEngine::Vector4* _color) {

	bool result = false;
	float width = 50.0f; // 各ボックスの横幅
	static bool openPicker = false;

	//ImGui::Text(_label);
	ImVec4 editColor = ImVec4(_color->x, _color->y, _color->z, _color->w);

	/// R
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.3f, 0.0f, 0.0f, 1.0f));
	ImGui::SetNextItemWidth(width);
	ImGui::DragFloat(std::format("##R{:p}", reinterpret_cast<void*>(&_label)).c_str(), &editColor.x, 0.01f, 0.0f, 1.0f, "R: %.2f");
	ImGui::PopStyleColor();
	ImGui::SameLine();

	/// G
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.3f, 0.0f, 1.0f));
	ImGui::SetNextItemWidth(width);
	ImGui::DragFloat(std::format("##G{:p}", reinterpret_cast<void*>(&_label)).c_str(), &editColor.y, 0.01f, 0.0f, 1.0f, "G: %.2f");
	ImGui::PopStyleColor();
	ImGui::SameLine();

	/// B
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 0.3f, 1.0f));
	ImGui::SetNextItemWidth(width);
	ImGui::DragFloat(std::format("##B{:p}", reinterpret_cast<void*>(&_label)).c_str(), &editColor.z, 0.01f, 0.0f, 1.0f, "B: %.2f");
	ImGui::PopStyleColor();
	ImGui::SameLine();

	/// A
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
	ImGui::SetNextItemWidth(width);
	ImGui::DragFloat(std::format("##A{:p}", reinterpret_cast<void*>(&_label)).c_str(), &editColor.w, 0.01f, 0.0f, 1.0f, "A: %.2f");
	ImGui::PopStyleColor();
	ImGui::SameLine();

	/// 色のプレビュー
	if(ImGui::ColorButton(std::format("##Preview{}", *_label).c_str(), editColor, ImGuiColorEditFlags_NoTooltip, ImVec2(30, 0))) {
		openPicker = !openPicker;
	}

	if(openPicker) {
		ImGui::ColorPicker4(std::format("##Picker{}", *_label).c_str(), (float*)&editColor, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoLabel);
	}

	/// 色を更新
	if(editColor.x != _color->x || editColor.y != _color->y || editColor.z != _color->z || editColor.w != _color->w) {
		_color->x = editColor.x;
		_color->y = editColor.y;
		_color->z = editColor.z;
		_color->w = editColor.w;
		result = true;
	}

	return result;
}

void ONEngine::DirectionalLightDebug(DirectionalLight* _light) {
	if(!_light) {
		return;
	}

	/// param get
	float intensity = _light->GetIntensity();
	Vector4  color = _light->GetColor();
	Vector3  direction = _light->GetDirection();

	/// edit
	if(ImGuiColorEdit("color", &color)) {
		_light->SetColor(color);
	}

	if(ImGui::DragFloat3("direction", &direction.x, 0.1f)) {
		_light->SetDirection(Vector3::Normalize(direction));
	}

	if(ImGui::DragFloat("intensity", &intensity, 0.1f)) {
		_light->SetIntensity(intensity);
	}

}

void ONEngine::AudioSourceDebug(AudioSource* _audioSource) {
	if(!_audioSource) {
		return;
	}

	/// param get
	float volume = _audioSource->GetVolume();
	float pitch = _audioSource->GetPitch();
	std::string path = _audioSource->GetAudioPath();

	/// edit
	if(ImGui::DragFloat("volume", &volume, 0.1f)) {
		_audioSource->SetVolume(volume);
	}

	if(ImGui::DragFloat("pitch", &pitch, 0.1f)) {
		_audioSource->SetPitch(pitch);
	}

}

void ONEngine::CustomMeshRendererDebug(CustomMeshRenderer* _customMeshRenderer) {
	if(!_customMeshRenderer) {
		return;
	}

	/// param get
	Vector4 color = _customMeshRenderer->GetColor();
	/// edit
	if(ImGuiColorEdit("color", &color)) {
		_customMeshRenderer->SetColor(color);
	}
}

void ONEngine::EffectDebug(Effect* _effect) {
	if(!_effect) {
		return;
	}

	/// main module
	if(ImGui::TreeNodeEx("main module", ImGuiTreeNodeFlags_DefaultOpen)) {
		EffectMainModule* mainModule = _effect->GetMainModule();
		if(!mainModule) {
			ImGui::Text("no main module");
		} else {

			/// param get
			std::pair<float, float> speed = mainModule->GetSpeedStartData();
			std::pair<Vector3, Vector3> size = mainModule->GetSizeStartData();
			std::pair<Vector3, Vector3> rotate = mainModule->GetRotateStartData();
			std::pair<Vector4, Vector4> color = mainModule->GetColorStartData();

			/// スピードの編集
			ImGui::DragFloat("first speed", &speed.first, 0.1f, 0.0f, FLT_MAX);
			ImGui::DragFloat("second speed", &speed.second, 0.1f, 0.0f, FLT_MAX);
			ImGui::Spacing();

			/// サイズの編集
			ImGui::DragFloat3("first size", &size.first.x, 0.1f, 0.0f, FLT_MAX);
			ImGui::DragFloat3("second size", &size.second.x, 0.1f, 0.0f, FLT_MAX);
			ImGui::Spacing();

			/// 回転の編集
			ImGui::DragFloat3("first rotate", &rotate.first.x, 0.1f);
			ImGui::DragFloat3("second rotate", &rotate.second.x, 0.1f);
			ImGui::Spacing();

			/// 色の編集
			ImGui::ColorEdit4("first color", &color.first.x);
			ImGui::ColorEdit4("second color", &color.second.x);

		}

		ImGui::TreePop();
	}

}


