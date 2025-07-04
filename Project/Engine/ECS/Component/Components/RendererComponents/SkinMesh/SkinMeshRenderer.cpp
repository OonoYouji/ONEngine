#include "SkinMeshRenderer.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/Core/Utility/Utility.h"


SkinMeshRenderer::SkinMeshRenderer() {
	SetMeshPath("./Assets/Models/primitive/cube.obj");
	SetTexturePath("./Packages/Textures/white.png");
	animationTime_ = 0.0f;
	duration_ = 0.0f;
}

void SkinMeshRenderer::SetMeshPath(const std::string& _path) {
	meshPath_ = _path;
}

void SkinMeshRenderer::SetTexturePath(const std::string& _path) {
	texturePath_ = _path;
}

void SkinMeshRenderer::SetAnimationTime(float _time) {
	animationTime_ = _time;
}

void SkinMeshRenderer::SetDuration(float _duration) {
	duration_ = _duration;
}

const std::string& SkinMeshRenderer::GetMeshPath() const {
	return meshPath_;
}

const std::string& SkinMeshRenderer::GetTexturePath() const {
	return texturePath_;
}

float SkinMeshRenderer::GetAnimationTime() const {
	return animationTime_;
}

float SkinMeshRenderer::GetDuration() const {
	return duration_;
}




void COMP_DEBUG::SkinMeshRendererDebug(SkinMeshRenderer* _smr) {
	if(_smr == nullptr) {
		return;
	}

	/// param get
	std::string meshPath = _smr->GetMeshPath();
	std::string texturePath = _smr->GetTexturePath();

	float animationTime = _smr->GetAnimationTime();
	float duration = _smr->GetDuration();

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