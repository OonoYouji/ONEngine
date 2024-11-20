#include "ModelPreviewObject.h"

/// std
#include <format>

/// externals
#include <imgui.h>

/// engine
#include <Input/Input.h>

/// component
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

ModelPreviewObject::ModelPreviewObject(const std::string& _modelFilePath) : modelFilePath_(_modelFilePath) {
	CreateTag(this);
}

ModelPreviewObject::~ModelPreviewObject() {}

void ModelPreviewObject::Initialize() {

	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel(modelFilePath_);

}

void ModelPreviewObject::Update() {

}

void ModelPreviewObject::Debug() {
	std::vector<char> buffer(modelFilePath_.begin(), modelFilePath_.end());
	buffer.resize(512, '\0');

	ImGui::Text("\"./Resources/Models/\"");
	ImGui::Text(std::format("current path\"{}\"", modelFilePath_).c_str());
	ImGui::InputText("modelFilePath", buffer.data(), buffer.size());

	if(Input::TriggerKey(KeyCode::Return)) {
		modelFilePath_ = std::string(buffer.data());
		meshRenderer_->SetModel(modelFilePath_);
	}
	
}

