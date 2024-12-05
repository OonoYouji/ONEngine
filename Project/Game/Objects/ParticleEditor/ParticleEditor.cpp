#include "ParticleEditor.h"

/// std
#include <filesystem>

/// externals
#include <imgui.h>

/// engine
#include "ComponentManager/ParticleSystem/ParticleSystem.h"


namespace {

	int InputTextCallback(ImGuiInputTextCallbackData* data) {
		if(data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
			// std::stringをリサイズ
			auto* str = static_cast<std::string*>(data->UserData);
			str->resize(data->BufTextLen);
			data->Buf = str->data();
		}
		return 0;
	}

} /// namespace



ParticleEditor::ParticleEditor() {
	CreateTag(this);
}

ParticleEditor::~ParticleEditor() {}

void ParticleEditor::Initialize() {

	particleSystem_ = AddComponent<ParticleSystem>(1024, "Sphere");

	directoryPath_ = "./Resources/Parameters/Particle";
	fileName_ = "";

}

void ParticleEditor::Update() {

}

void ParticleEditor::Debug() {
	if(ImGui::TreeNode("editor")) {

		// バッファのサイズ管理を委ねる
		ImGui::InputText(
			"saved file name", fileName_.data(), fileName_.capacity() + 1,
			ImGuiInputTextFlags_CallbackResize, InputTextCallback, &fileName_
		);


		{	/// ファイルの探索、セーブするファイルを変更する
			currentFiles_ = SearchFile(directoryPath_, "json");

			/// 現在選択中の項目インデックス
			static int currentIndex = 0;
			/// std::list<std::string>からconst char*の配列を生成
			std::vector<const char*> items;
			for(const auto& option : currentFiles_) {
				items.push_back(option.second.c_str());
			}

			// ImGui::Comboの表示
			if(ImGui::Combo("current files", &currentIndex, items.data(), static_cast<int>(items.size()))) {
				fileName_ = items[currentIndex];
			}
		}


		if(ImGui::Button("save file")) {

		}



		ImGui::TreePop();
	}


}


std::list<std::pair<std::string, std::string>> ParticleEditor::SearchFile(const std::string& directory, const std::string& extension) {
	namespace fs = std::filesystem;

	std::list<std::pair<std::string, std::string>> fileList;
	for(const auto& entry : fs::recursive_directory_iterator(directory)) {
		if(entry.is_regular_file() && entry.path().extension() == ('.' + extension)) {
			fileList.push_back({ entry.path().parent_path().string(),entry.path().filename().stem().string() });
		}
	}
	return fileList;
}