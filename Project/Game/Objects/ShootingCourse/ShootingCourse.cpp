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
#include "ComponentManager/MeshInstancingRenderer/MeshInstancingRenderer.h"

/// using namespace
using namespace nlohmann;


ShootingCourse::ShootingCourse() {
	CreateTag(this);
}

ShootingCourse::~ShootingCourse() {}

void ShootingCourse::Initialize() {
	splinePathRenderer_    = AddComponent<SplinePathRenderer>(6);
	meshInstancedRenderer_ = AddComponent<MeshInstancingRenderer>(128);

	LoadFile(filePath_);
	splinePathRenderer_->SetAnchorPointArray(vertices_);

	/// 読み込んだポイントが4個以下であれば
	if(anchorPointArray_.size() < 4) {
		AddAnchorPoint({});
		AddAnchorPoint({});
		AddAnchorPoint({});
		AddAnchorPoint({});
	}


	std::vector<Vec3> tmpVertices;
	for(auto& anchorPoint : anchorPointArray_) {
		tmpVertices.push_back(anchorPoint.position);
	}
	vertices_ = tmpVertices;

	for(auto& vertex : vertices_) {
		transformArray_.push_back(Transform());
		Transform& transform = transformArray_.back();
		transform.position = vertex;
		transform.rotate   = { 0,0,0 };
		transform.scale    = { 1,1,1};
		transform.UpdateMatrix(false);

		meshInstancedRenderer_->AddTransform(&transform);
	}

}

void ShootingCourse::Update() {

#ifdef _DEBUG	/// copy, 重そう、 debugのときだけにする
	std::vector<Vec3> tmpVertices;
	for(auto& anchorPoint : anchorPointArray_) {
		tmpVertices.push_back(anchorPoint.position);
	}
	vertices_ = tmpVertices;
#endif // _DEBUG

	std::vector<Transform*> transformPtrArray{};
	for(auto& transform : transformArray_) {
		transformPtrArray.push_back(&transform);
	}

	meshInstancedRenderer_->SetTransformArray(transformPtrArray);

	splinePathRenderer_->isActive = (vertices_.size() >= 4);
	splinePathRenderer_->SetAnchorPointArray(vertices_);
}


void ShootingCourse::Debug() {
	if(ImGui::TreeNodeEx("course editor", ImGuiTreeNodeFlags_DefaultOpen)) {

		if(ImGui::TreeNodeEx("anchor points", ImGuiTreeNodeFlags_DefaultOpen)) {

			if(ImGui::Button("save file")) {
				SaveFile(filePath_);
			}

			ImGui::Separator();


			ImGui::SliderInt("addIndex", &addIndex_, 0, static_cast<int>(anchorPointArray_.size() - 1));
			if(ImGui::Button("add")) {
				auto itr = anchorPointArray_.begin() + addIndex_;
				anchorPointArray_.insert(itr, (*itr));
			}

			ImGui::Separator();


			ImGui::SliderInt("subtractIndex", &subtractIndex_, 0, static_cast<int>(anchorPointArray_.size() - 1));
			if(ImGui::Button("subtract")) {
				if(subtractIndex_ >= anchorPointArray_.size()) {
					subtractIndex_ = static_cast<int>(anchorPointArray_.size() - 1);
				}
				if(anchorPointArray_.size() > 4) {
					anchorPointArray_.erase(anchorPointArray_.begin() + subtractIndex_);
				}
			}


			ImGui::TreePop();
		}

		ImGui::TreePop();
	}


	ImGui::BeginChild("anchor point array scroll bar", ImVec2(0, 360.0f), true, ImGuiWindowFlags_HorizontalScrollbar);

	ImGui::SetNextItemOpen(true, ImGuiCond_Always);
	if(ImGui::TreeNodeEx("anchor points", ImGuiTreeNodeFlags_DefaultOpen)) {

		uint32_t index = 0u;
		for(auto& anchorPoint : anchorPointArray_) {

			std::string positionLabel = std::string("position_") + std::to_string(index);
			std::string upLabel       = std::string("up_") + std::to_string(index);

			ImGui::DragFloat3(positionLabel.c_str(), &anchorPoint.position.x, 0.05f);
			ImGui::DragFloat3(upLabel.c_str(),       &anchorPoint.up.x,       0.05f);

			ImGui::Spacing();
			index++;
		}

		ImGui::TreePop();
	}

	ImGui::EndChild();


}



void ShootingCourse::SaveFile(const std::string& filePath) {
	json root = json::object();
	for(size_t i = 0; i < anchorPointArray_.size(); ++i) {
		auto& item = root[std::to_string(i)];

		AnchorPoint& point = anchorPointArray_[i];
		item["position"] = json::array({ point.position.x, point.position.y, point.position.z });
		item["up"]       = json::array({ point.up.x,       point.up.y,       point.up.z });
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


	for(auto& item : root.items()) {

		auto jsonPos = item.value()["position"];
		auto jsonUp  = item.value()["up"];
		AnchorPoint anchorPoint{
			.position = {jsonPos.at(0), jsonPos.at(1), jsonPos.at(2)},
			.up       = {jsonUp.at(0),  jsonUp.at(1),  jsonUp.at(2)}
		};

		anchorPointArray_.push_back(anchorPoint);
	}


}

void ShootingCourse::AddAnchorPoint(const Vec3& point) {
	anchorPointArray_.emplace_back(point);
}
