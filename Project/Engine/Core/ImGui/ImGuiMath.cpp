#include "ImGuiMath.h"

/// std
#include <numbers>

/// engine
#include "Engine/ECS/Component/Component.h"


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

	ImGui::Text(_label);
	ImVec4 myColor = ImVec4(_color->x, _color->y, _color->z, _color->w);

	/// R
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.3f, 0.0f, 0.0f, 1.0f));
	ImGui::SetNextItemWidth(width);
	ImGui::DragFloat("##R", &myColor.x, 0.01f, 0.0f, 1.0f, "R: %.2f");
	ImGui::PopStyleColor();
	ImGui::SameLine();

	/// G
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.3f, 0.0f, 1.0f));
	ImGui::SetNextItemWidth(width);
	ImGui::DragFloat("##G", &myColor.y, 0.01f, 0.0f, 1.0f, "G: %.2f");
	ImGui::PopStyleColor();
	ImGui::SameLine();

	/// B
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 0.3f, 1.0f));
	ImGui::SetNextItemWidth(width);
	ImGui::DragFloat("##B", &myColor.z, 0.01f, 0.0f, 1.0f, "B: %.2f");
	ImGui::PopStyleColor();
	ImGui::SameLine();

	/// A
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
	ImGui::SetNextItemWidth(width);
	ImGui::DragFloat("##A", &myColor.w, 0.01f, 0.0f, 1.0f, "A: %.2f");
	ImGui::PopStyleColor();
	ImGui::SameLine();


	/// 色のプレビュー
	if (ImGui::ColorButton("##Preview", myColor, ImGuiColorEditFlags_NoTooltip, ImVec2(30, 0))) {
		openPicker = !openPicker;
	}

	if (openPicker) {
		ImGui::ColorPicker4("##Picker", (float*)&myColor, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoLabel);
	}


	/// 色を更新
	if (myColor.x != _color->x || myColor.y != _color->y || myColor.z != _color->z || myColor.w != _color->w) {
		_color->x = myColor.x;
		_color->y = myColor.y;
		_color->z = myColor.z;
		_color->w = myColor.w;
		result = true;
	}


	return result;
}

void TransformDebug(Transform* _transform) {
	if (!_transform) {
		return;
	}

	ImGui::DragFloat3("position", &_transform->position.x, 0.1f);
	ImGui::DragFloat3("rotate",   &_transform->rotate.x,   rotateSpeed);
	ImGui::DragFloat3("scale",    &_transform->scale.x,    0.1f);
}

void DirectionalLightDebug(DirectionalLight* _light) {
	if (!_light) {
		return;
	}

	/// param get
	float intensity = _light->GetIntensity();
	Vec4  color     = _light->GetColor();
	Vec3  direction = _light->GetDirection();
	
	/// edit
	if (ImGui::ColorEdit4("color", &color.x)) {
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
	float pitch  = _audioSource->GetPitch();
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
	
	/// edit
	if (ImGuiColorEdit("color", &color)) {
		_customMeshRenderer->SetColor(color);
	}
}
