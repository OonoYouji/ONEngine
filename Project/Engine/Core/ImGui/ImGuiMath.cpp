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

}	/// unnamed namespace


void ImGuiInputText(const char* _label, std::string* _text) {
	ImGui::InputText(
		_label,
		(*_text).data(), (*_text).capacity(),
		ImGuiInputTextFlags_CallbackAlways,
		[](ImGuiInputTextCallbackData* _data) {
			auto* str = static_cast<std::string*>(_data->UserData);
			str->assign(_data->Buf, _data->BufTextLen);
			return 0;
		}, _text
	);
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

void TransformDebug(Transform* _transform) {
	if (!_transform) {
		return;
	}

	ImGui::DragFloat3("position", &_transform->position.x, 0.1f);
	ImGui::DragFloat3("rotate", &_transform->rotate.x, rotateSpeed);
	ImGui::DragFloat3("scale", &_transform->scale.x, 0.1f);
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

void VariablesDebug(Variables* _variables) {
	if (!_variables) {
		return;
	}

	std::list<std::pair<std::string, std::string>> removeList;
	std::vector<std::tuple<std::string, Variables::Var, std::string>> variables;
	std::string ptrStr, label;

	for (const auto& [key, index] : _variables->GetKeyMap()) {
		variables.emplace_back(key, _variables->GetVariables()[index], "##{:p}" + std::to_string(reinterpret_cast<uintptr_t>(&_variables->GetVariables()[index])));
	}

	for (auto& [name, variable, str] : variables) {
		ptrStr = str;
		label = name;

		ImGui::SetNextItemWidth(64.0f);
		if (ImGui::InputText((ptrStr + "string").c_str(), label.data(), label.capacity())) {
			label.resize(strlen(label.c_str()));
			removeList.push_back({ name, label });
		}

		ImGui::SameLine();

		/// 変数の型によって処理を変える
		ValueImGui(_variables, ptrStr, name, variable.index());
	}

	for (auto& [oldName, newName] : removeList) {
		_variables->Rename(oldName, newName);
	}

	ImGui::Spacing();

	if (ImGui::Button("export")) {
		std::string ownerName = typeid(*_variables->GetOwner()).name();
		/// "class "を除去
		ownerName.erase(0, 6);

		_variables->SaveJson("Assets/Jsons/" + ownerName + ".json");
	}

	ImGui::SameLine();

	// open Dialog Simple
	if (ImGui::Button("Open File Dialog")) {
		IGFD::FileDialogConfig config;
		config.path = ".";
		ImGuiFileDialog::Instance()->OpenDialog("Dialog", "Choose File", ".json", config);
	}
	// display
	if (ImGuiFileDialog::Instance()->Display("Dialog", ImGuiWindowFlags_NoDocking)) {
		if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
			// action
			_variables->LoadJson(filePathName);
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}
}

void MeshRendererDebug(MeshRenderer* _meshRenderer) {
	if (!_meshRenderer) {
		return;
	}

	/// param get
	Vec4 color = _meshRenderer->GetColor();

	/// edit
	if (ImGuiColorEdit("color", &color)) {
		_meshRenderer->SetColor(color);
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

void ValueImGui(Variables* _variables, const std::string& _label, const std::string& _name, size_t _type) {
	{
		switch (_type) {
		case 0:
			ImGui::DragInt(_label.c_str(), &_variables->Get<int>(_name), 1, -INT_MAX, INT_MAX, "%d", ImGuiSliderFlags_AlwaysClamp);
			break;
		case 1:
			ImGui::DragFloat(_label.c_str(), &_variables->Get<float>(_name), 0.2f, -FLT_MAX, FLT_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp);
			break;
		case 2:
			ImGui::Checkbox(_label.c_str(), &_variables->Get<bool>(_name));
			break;
		case 3:
			ImGuiInputText(_label.c_str(), &_variables->Get<std::string>(_name));
			break;
		case 4:
			ImGui::DragFloat2(_label.c_str(), &_variables->Get<Vector2>(_name).x, 0.2f, -FLT_MAX, FLT_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp);
			break;
		case 5:
			ImGui::DragFloat3(_label.c_str(), &_variables->Get<Vector3>(_name).x, 0.2f, -FLT_MAX, FLT_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp);
			break;
		case 6:
			ImGui::DragFloat4(_label.c_str(), &_variables->Get<Vector4>(_name).x, 0.2f, -FLT_MAX, FLT_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp);
			break;
		}
	}
}
