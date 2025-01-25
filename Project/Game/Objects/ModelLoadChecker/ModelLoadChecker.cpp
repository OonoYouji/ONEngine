#include "ModelLoadChecker.h"

/// std
#include <filesystem>
#include <iostream>

/// externals
#include <imgui.h>

/// engine
#include "GraphicManager/ModelManager/ModelManager.h"

namespace fs = std::filesystem;

ModelLoadChecker::ModelLoadChecker() {
	CreateTag(this);
}

ModelLoadChecker::~ModelLoadChecker() {}

void ModelLoadChecker::Initialize() {

}

void ModelLoadChecker::Update() {

}

void ModelLoadChecker::Debug() {

	const std::string modelDirectory = "./Resources/Models/";

	for (const auto& entry : fs::directory_iterator(modelDirectory)) {
		const std::string&& modelName = entry.path().filename().string();
		bool&&              isLoaded  = ModelManager::IsModelLoaded(modelName);

		ImGui::Checkbox("##loaded", &isLoaded);
		ImGui::SameLine();
		if (ImGui::Button(modelName.c_str())) {
			ModelManager::Load(modelName);
		}
	}

}

