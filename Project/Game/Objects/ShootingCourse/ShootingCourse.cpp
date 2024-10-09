#include "ShootingCourse.h"

/// std
#include <cassert>
#include <fstream>
#include <sstream>

/// externals
#include <json.hpp>

/// engine
#include "ImGuiManager/ImGuiManager.h"

/// components
#include "ComponentManager/SplinePathRenderer/SplinePathRenderer.h"


/// using namespace
using namespace nlohmann;


ShootingCourse::ShootingCourse() {
	CreateTag(this);
}

ShootingCourse::~ShootingCourse() {}

void ShootingCourse::Initialize() {
	splinePathRenderer_ = AddComponent<SplinePathRenderer>(6);

	LoadFile(filePath_);
	splinePathRenderer_->SetAnchorPointArray(anchorPointArray_);

	/// 読み込んだポイントが4個以下であれば
	if(anchorPointArray_.size() < 4) {
		AddAnchorPoint({});
		AddAnchorPoint({});
		AddAnchorPoint({});
		AddAnchorPoint({});
	}


}

void ShootingCourse::Update() {
	splinePathRenderer_->SetAnchorPointArray(anchorPointArray_);
}


void ShootingCourse::Debug() {
	if(ImGui::TreeNodeEx("course editor", ImGuiTreeNodeFlags_DefaultOpen)) {

		if(ImGui::TreeNodeEx("anchor points", ImGuiTreeNodeFlags_DefaultOpen)) {

			if(ImGui::Button("save file")) {
				SaveFile(filePath_);
			}

			ImGui::Separator();


			if(ImGui::Button("add")) {
				Vec3& back = anchorPointArray_.back();
				anchorPointArray_.emplace_back(back);
			}


			ImGui::TreePop();
		}

		ImGui::TreePop();
	}

	if(ImGui::TreeNodeEx("anchor points", ImGuiTreeNodeFlags_DefaultOpen)) {

		uint32_t index = 0u;
		for(auto& anchorPoint : anchorPointArray_) {
			
			std::string label = std::string("position_") + std::to_string(index);
			ImGui::DragFloat3(label.c_str(), &anchorPoint.x, 0.05f);

			index++;
		}

		ImGui::TreePop();
	}
}



void ShootingCourse::SaveFile(const std::string& filePath) {
	json root = json::object();
	for(size_t i = 0; i < anchorPointArray_.size(); ++i) {
		Vec3& point = anchorPointArray_[i];
		root[std::to_string(i)] = json::array({ point.x, point.y, point.z});
	}

	///- ディレクトリがなければ作成する
	std::filesystem::path dir(filePath);
	if(!std::filesystem::exists(dir)) {
		std::filesystem::create_directories(dir);
	}


	///- File open
	std::string path = filePath + "ShootingCourse.json";
	std::ofstream ofs;
	ofs.open(path);

	if(ofs.fail()) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "ShootingCourse", 0);
		assert(false);
		return;
	}

	///- ファイルにjson文字列を書き込む(インデント幅4)
	ofs << std::setw(4) << root << std::endl;
	ofs.close();

}

void ShootingCourse::LoadFile(const std::string& filePath) {
	///- ファイルを開く
	std::string path = filePath + "ShootingCourse.json";
	std::ifstream ifs;
	ifs.open(path);

	///- 開けなければメッセージを出す
	if(!ifs.is_open()) {
		std::string message = "File could not be opened.";
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		assert(false);
		return;
	}

	///- json文字列からjsonのデータ構造に展開
	json root;
	ifs >> root;
	ifs.close();



}

void ShootingCourse::AddAnchorPoint(const Vec3& point) {
	anchorPointArray_.emplace_back(point);
	splinePathRenderer_->SetAnchorPointArray(anchorPointArray_);
}
