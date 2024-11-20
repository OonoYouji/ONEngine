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

}

void ModelPreviewObject::Update() {

}

void ModelPreviewObject::Debug() {
	
	if(ImGui::Button("research")) {
		searchFiles_ = myFs::SearchFile("./Resources/Models", "gltf");
	}


	ImGui::Text("\"./Resources/Models/\"");
	ImGui::Text(std::format("current path\"{}\"", modelFilePath_).c_str());


	static int currentItem = 0; // 現在選択されている項目のインデックス
	// ImGui::Comboに渡すために文字列リストを1つのC文字列に変換
	std::vector<const char*> itemCStrs;
	for(const auto& item : searchFiles_) {
		itemCStrs.push_back(item.second.c_str());
	}

	// Comboボックスの描画
	if(ImGui::Combo("Select Item", &currentItem, itemCStrs.data(), static_cast<int>(itemCStrs.size()))) {
		// 項目が選択された時の処理
		auto itr = std::next(searchFiles_.begin(), currentItem);

		modelFilePath_ = std::string((*itr).second.data());
		animationRenderer_->ChangeAnimation(modelFilePath_);
	}
	
}

