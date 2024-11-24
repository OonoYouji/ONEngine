#include "ModelPreviewObject.h"

/// std
#include <format>

/// externals
#include <imgui.h>

/// engine
#include <Input/Input.h>

/// component
#include "ComponentManager/AnimationRenderer/AnimationRenderer.h"

#include "MyFileSystem/MyFileSystem.h"
	
ModelPreviewObject::ModelPreviewObject(const std::string& _modelFilePath) : modelFilePath_(_modelFilePath) {
	CreateTag(this);
}

ModelPreviewObject::~ModelPreviewObject() {}

void ModelPreviewObject::Initialize() {

	animationRenderer_ = AddComponent<AnimationRenderer>(modelFilePath_);
	animationRenderer_->SetModel(modelFilePath_);

	searchFiles_ = myFs::SearchFile("./Resources/Models", "gltf");

	totalTime_ = 10.0f;

}

void ModelPreviewObject::Update() {

}

void ModelPreviewObject::Debug() {
	

	
	if(ImGui::Checkbox("isStopAnimation", &isStopAnimation_)) {
		animationRenderer_->SetIsStopAnimation(isStopAnimation_);
	}

	ImGui::Spacing();

	if(ImGui::Button("research")) {
		searchFiles_ = myFs::SearchFile("./Resources/Models", "gltf");
	}


	ImGui::Text("\"./Resources/Models/\"");
	ImGui::Text(std::format("current path\"{}\"", modelFilePath_).c_str());


	static int currentItem = 0; 
	std::vector<const char*> itemCStrs;
	for(const auto& item : searchFiles_) {
		itemCStrs.push_back(item.second.c_str());
	}

	if(ImGui::Combo("Select Item", &currentItem, itemCStrs.data(), static_cast<int>(itemCStrs.size()))) {
		// 項目が選択された時の処理
		auto itr = std::next(searchFiles_.begin(), currentItem);

		modelFilePath_ = std::string((*itr).second.data());
		animationRenderer_->ChangeAnimation(modelFilePath_);
	}
	


	/// total time
	ImGui::DragFloat("animation totla time", &totalTime_, 0.1f);

	if(ImGui::Button("set total time")) {
		animationRenderer_->SetTotalTime(totalTime_, modelFilePath_);
	}

}

