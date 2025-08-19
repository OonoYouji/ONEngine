#include "ImGuiMath.h"

/// std
#include <numbers>
#include <format>
#include <variant>

/// external
#include <Externals/imgui/dialog/ImGuiFileDialog.h>

/// engine
#include "Engine/ECS/Component/Component.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

namespace {

	float rotateSpeed = std::numbers::pi_v<float> / 100.0f;

	std::string variableName = "";

}	/// unnamed namespace


ImVec4 ImMathf::ToImVec4(const Vector4& _vec) {
	return ImVec4(_vec.x, _vec.y, _vec.z, _vec.w);
}

ImVec2 ImMathf::ToImVec2(const Vector2& _vec) {
	return ImVec2(_vec.x, _vec.y);
}

bool ImMathf::ColorEdit(const char* _label, Vector4* _color, ImGuiColorEditFlags _flags) {
	if (!_color) {
		return false;
	}

	if (ImGui::ColorEdit4(_label, &_color->x, _flags)) {
		return true; // 色が変更された
	}

	return false;
}

bool ImMathf::InputText(const char* _label, std::string* _text, ImGuiInputTextFlags _flags) {
	if (!_text) {
		return false; // nullptr check
	}

	_flags |= ImGuiInputTextFlags_CallbackResize;

	struct CallbackUserData {
		std::string* text;
	};

	auto callback = [](ImGuiInputTextCallbackData* data) -> int {
		if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
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


bool ImGuiInputText(const char* _label, std::string* _text, ImGuiInputTextFlags _flags) {
	if (!_text) {
		return false; // nullptr check
	}

	_flags |= ImGuiInputTextFlags_CallbackResize;

	struct CallbackUserData {
		std::string* text;
	};

	auto callback = [](ImGuiInputTextCallbackData* data) -> int {
		if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
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

void ImGuiInputTextReadOnly(const char* _label, const std::string& _text) {
	char buffer[256];
	strncpy_s(buffer, _text.c_str(), sizeof(buffer));
	buffer[sizeof(buffer) - 1] = '\0';
	ImGui::InputText(_label, buffer, sizeof(buffer), ImGuiInputTextFlags_ReadOnly);
}

bool ImGuiColorEdit(const char* _label, Vector4* _color) {

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
	if (ImGui::ColorButton(std::format("##Preview{}", *_label).c_str(), editColor, ImGuiColorEditFlags_NoTooltip, ImVec2(30, 0))) {
		openPicker = !openPicker;
	}

	if (openPicker) {
		ImGui::ColorPicker4(std::format("##Picker{}", *_label).c_str(), (float*)&editColor, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoLabel);
	}

	/// 色を更新
	if (editColor.x != _color->x || editColor.y != _color->y || editColor.z != _color->z || editColor.w != _color->w) {
		_color->x = editColor.x;
		_color->y = editColor.y;
		_color->z = editColor.z;
		_color->w = editColor.w;
		result = true;
	}

	return result;
}

void DirectionalLightDebug(DirectionalLight* _light) {
	if (!_light) {
		return;
	}

	/// param get
	float intensity = _light->GetIntensity();
	Vec4  color = _light->GetColor();
	Vec3  direction = _light->GetDirection();

	/// edit
	if (ImGuiColorEdit("color", &color)) {
		_light->SetColor(color);
	}

	if (ImGui::DragFloat3("direction", &direction.x, 0.1f)) {
		_light->SetDirection(Vec3::Normalize(direction));
	}

	if (ImGui::DragFloat("intensity", &intensity, 0.1f)) {
		_light->SetIntensity(intensity);
	}

}

void AudioSourceDebug(AudioSource* _audioSource) {
	if (!_audioSource) {
		return;
	}

	/// param get
	float volume = _audioSource->GetVolume();
	float pitch = _audioSource->GetPitch();
	std::string path = _audioSource->GetAudioPath();

	/// edit
	if (ImGui::DragFloat("volume", &volume, 0.1f)) {
		_audioSource->SetVolume(volume);
	}

	if (ImGui::DragFloat("pitch", &pitch, 0.1f)) {
		_audioSource->SetPitch(pitch);
	}

	//ImGuiInputText("path", &path);
	//_audioSource->SetAudioPath(path);
	////if (ImGui::InputText("path", &path)) {
	////	_audioSource->SetAudioPath(path);
	////}

}

void MeshRendererDebug(MeshRenderer* _meshRenderer) {
	if (!_meshRenderer) {
		return;
	}

	/// param get
	Vec4 color = _meshRenderer->GetColor();
	std::string meshPath = _meshRenderer->GetMeshPath();
	std::string texturePath = _meshRenderer->GetTexturePath();

	/// edit
	if (ImGuiColorEdit("color", &color)) {
		_meshRenderer->SetColor(color);
	}


	ImGui::Spacing();


	/// meshの変更
	ImGui::Text("mesh path");
	ImGui::InputText("##mesh", meshPath.data(), meshPath.capacity(), ImGuiInputTextFlags_ReadOnly);
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetData")) {
			if (payload->Data) {
				const char* droppedPath = static_cast<const char*>(payload->Data);
				std::string path = std::string(droppedPath);

				if (path.find(".obj") != std::string::npos
					|| path.find(".gltf") != std::string::npos) {
					_meshRenderer->SetMeshPath(path);

					Console::Log(std::format("Mesh path set to: {}", path));
				} else {
					Console::Log("Invalid mesh format. Please use .obj or .gltf.");
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
			if (payload->Data) {
				const char* droppedPath = static_cast<const char*>(payload->Data);
				std::string path = std::string(droppedPath);

				if (path.find(".png") != std::string::npos
					|| path.find(".jpg") != std::string::npos
					|| path.find(".jpeg") != std::string::npos) {
					_meshRenderer->SetTexturePath(path);

					Console::Log(std::format("Texture path set to: {}", path));
				} else {
					Console::Log("Invalid texture format. Please use .png, .jpg, or .jpeg.");
				}
			}
		}

		ImGui::EndDragDropTarget();
	}

}

void CustomMeshRendererDebug(CustomMeshRenderer* _customMeshRenderer) {
	if (!_customMeshRenderer) {
		return;
	}

	/// param get
	Vec4 color = _customMeshRenderer->GetColor();
	//ImGuiColorEdit("colorあ", &color);
	/// edit
	if (ImGuiColorEdit("color", &color)) {
		_customMeshRenderer->SetColor(color);
	}
}

void EffectDebug(Effect* _effect) {
	if (!_effect) {
		return;
	}

	/// main module
	if (ImGui::TreeNodeEx("main module", ImGuiTreeNodeFlags_DefaultOpen)) {
		EffectMainModule* mainModule = _effect->GetMainModule();
		if (!mainModule) {
			ImGui::Text("no main module");
		} else {

			/// param get
			std::pair<float, float> speed = mainModule->GetSpeedStartData();
			std::pair<Vec3, Vec3> size = mainModule->GetSizeStartData();
			std::pair<Vec3, Vec3> rotate = mainModule->GetRotateStartData();
			std::pair<Vec4, Vec4> color = mainModule->GetColorStartData();

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


