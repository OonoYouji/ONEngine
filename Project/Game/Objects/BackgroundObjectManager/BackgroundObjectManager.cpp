#include "BackgroundObjectManager.h"

/// std
#include <format>
#include <iostream>
#include <fstream>

/// externals
#include <imgui.h>
#include <json.hpp>

/// lib
#include "Debugger/Assertion.h"

/// component
#include "ComponentManager/MeshInstancingRenderer/MeshInstancingRenderer.h"

/// objects
#include "Objects/ShootingCourse/ShootingCourse.h"
#include "Objects/RailCamera/RailCamera.h"
#include "MovingLight/MovingLight.h"


using namespace nlohmann;


BackgroundObjectManager::BackgroundObjectManager() {
	CreateTag(this);
}

BackgroundObjectManager::~BackgroundObjectManager() {}

void BackgroundObjectManager::Initialize() {

	startedTRenderer_       = AddComponent<MeshInstancingRenderer>(128);
	offsetPositionRenderer_ = AddComponent<MeshInstancingRenderer>(128);
	offsetPositionRenderer_->SetModel("Arrow");

	createClassNameArray_ = {
		"MovingLight"
	};

	LoadJsonFile("./Resources/Parameters/BackgroundObjectManager/");
	popObjectDataArray_ = objectDataArray_;

}

void BackgroundObjectManager::Update() {

	CalcuationObjectDataStartedTransform();
	CalcuationObjectDataOffsetPositionTransform();

	PopBBObject();

}

void BackgroundObjectManager::Debug() {

	ImGui::SeparatorText("object data debug");

	ImGui::DragFloat3("offset position", &srcObjectData_.offsetPosition.x, 0.1f);
	ImGui::DragFloat("startedT",         &srcObjectData_.startedT,         0.1f);


	std::vector<const char*> cStrings;
	for(const auto& item : createClassNameArray_) {
		cStrings.push_back(item.c_str());
	}

	if(ImGui::Combo("Select an option", &currentStringIndex_, cStrings.data(), static_cast<int>(cStrings.size()))) {
		srcObjectData_.createClassName = createClassNameArray_[currentStringIndex_];
	}


	ImGui::Spacing();

	

	if(ImGui::Button("save")) {
		SaveJsonFile("./Resources/Parameters/BackgroundObjectManager/");
	}

	ImGui::SameLine();
	ImGui::Text("/");
	ImGui::SameLine();

	if(ImGui::Button("load")) {
		LoadJsonFile("./Resources/Parameters/BackgroundObjectManager/");
	}

	ImGui::Spacing();



	if(!objectDataArray_.empty()) {
		operatorIndex_ = std::clamp(operatorIndex_, 0, static_cast<int>(objectDataArray_.size()));
	}

	ImGui::SliderInt("operatorIndex", &operatorIndex_, 0, static_cast<int>(objectDataArray_.size()));

	if(ImGui::Button("add")) {
		objectDataArray_.insert(objectDataArray_.begin() + operatorIndex_, srcObjectData_);
	}
	
	ImGui::SameLine();
	ImGui::Text("/");
	ImGui::SameLine();

	if(ImGui::Button("sub")) {
		if(!objectDataArray_.empty()) {
			objectDataArray_.erase(objectDataArray_.begin() + operatorIndex_);
		}
	}

	if(ImGui::Button("reload")) {
		popObjectDataArray_ = objectDataArray_;
	}



	/// スクロール
	ImGui::BeginChild("object data", ImVec2(0.0f, 320.0f), true, ImGuiWindowFlags_AlwaysHorizontalScrollbar);

	for(auto& data : objectDataArray_) {
		
		ImGui::DragFloat3(
			std::format("offset position ##{:p}", reinterpret_cast<void*>(&data)).c_str(),
			&data.offsetPosition.x, 0.1f
		);

		ImGui::DragFloat(
			std::format("startedT ##{:p}", reinterpret_cast<void*>(&data)).c_str(),
			&data.startedT, 0.1f
		);
		
		ImGui::Text(std::format("{} ##{:p}", 
			data.createClassName.c_str(), reinterpret_cast<void*>(&data)).c_str()
		);


		ImGui::Spacing();
	}

	ImGui::EndChild();
}



void BackgroundObjectManager::CalcuationObjectDataStartedTransform() {
	const std::vector<AnchorPoint>& anchorPoints = pShootingCourse_->GetAnchorPointArray();

	AnchorPoint ap{};
	for(size_t i = 0; i < objectDataArray_.size(); ++i) {
		ap = SplinePosition(anchorPoints, 1.0f / static_cast<float>(anchorPoints.size()) * objectDataArray_[i].startedT);

		if(i >= objectDataStartedTTransformArray_.size()) {

			Transform transform;
			transform.position = ap.position;
			transform.UpdateMatrix();
			objectDataStartedTTransformArray_.push_back(transform);

		} else {

			objectDataStartedTTransformArray_[i].position = ap.position;
			objectDataStartedTTransformArray_[i].UpdateMatrix();

		}
	}


	/// mesh instancing rendererに渡す
	startedTRenderer_->ResetTransformArray();
	for(auto& transform : objectDataStartedTTransformArray_) {
		startedTRenderer_->AddTransform(&transform);
	}
}

void BackgroundObjectManager::CalcuationObjectDataOffsetPositionTransform() {
	const std::vector<AnchorPoint>& anchorPoints = pShootingCourse_->GetAnchorPointArray();

	AnchorPoint ap{};
	for(size_t i = 0; i < objectDataArray_.size(); ++i) {
		ap = SplinePosition(anchorPoints, 1.0f / static_cast<float>(anchorPoints.size()) * objectDataArray_[i].startedT);
		ap.position += objectDataArray_[i].offsetPosition;

		if(i >= objectDataOffsetPositionTransformArray_.size()) {

			Transform transform;
			transform.position = ap.position;
			transform.UpdateMatrix();
			objectDataOffsetPositionTransformArray_.push_back(transform);

		} else {

			objectDataOffsetPositionTransformArray_[i].position = ap.position;
			objectDataOffsetPositionTransformArray_[i].UpdateMatrix();

		}
	}


	/// mesh instancing rendererに渡す
	offsetPositionRenderer_->ResetTransformArray();
	for(auto& transform : objectDataOffsetPositionTransformArray_) {
		offsetPositionRenderer_->AddTransform(&transform);
	}
}



void BackgroundObjectManager::PopBBObject() {
	const std::vector<AnchorPoint>& aps = pShootingCourse_->GetAnchorPointArray();
	AnchorPoint ap{};

	float movingTime = pRailCamera_->GetMovingTime();
	float preMovingTime = pRailCamera_->GetPreMovingTime();

	for(auto itr = popObjectDataArray_.begin(); itr != popObjectDataArray_.end();) {
		ObjectData& data = (*itr);

		if(movingTime > data.startedT && data.startedT > preMovingTime) {
			float t = 1.0f / static_cast<float>(aps.size()) * data.startedT;
			ap = SplinePosition(aps, t);

			CreateBBObject(
				data.createClassName, 
				ap.position + data.offsetPosition
			);

			itr = popObjectDataArray_.erase(itr);
		} else {
			itr++;
		}
	}
}



void BackgroundObjectManager::SaveJsonFile(const std::string& _filePath) {
	json root = json::object();
	for(size_t i = 0; i < objectDataArray_.size(); ++i) {
		auto& item = root[std::to_string(i)];

		ObjectData& data = objectDataArray_[i];
		item["offsetPosition"]  = json::array({ data.offsetPosition.x, data.offsetPosition.y, data.offsetPosition.z });
		item["startedT"]        = data.startedT;
		item["createClassName"] = data.createClassName;
	}

	///- ディレクトリがなければ作成する
	std::filesystem::path dir(_filePath);
	if(!std::filesystem::exists(dir)) {
		std::filesystem::create_directories(dir);
	}

	///- File open
	std::string path = _filePath + "BackgroundObjectManager.json";
	std::ofstream ofs;
	ofs.open(path);

	if(ofs.fail()) {
		Assert(false, "Failed open data file for write.");
		return;
	}

	///- ファイルにjson文字列を書き込む(インデント幅4)
	ofs << std::setw(4) << root << std::endl;
	ofs.close();
}

void BackgroundObjectManager::LoadJsonFile(const std::string& _filePath) {
	///- ファイルを開く
	std::string path = _filePath + "BackgroundObjectManager.json";
	std::ifstream ifs;
	ifs.open(path);

	///- 開けなければメッセージを出す
	if(!ifs.is_open()) {
		Assert(false, "File could not be opened.");
		return;
	}

	///- json文字列からjsonのデータ構造に展開
	json root;
	ifs >> root;
	ifs.close();


	size_t ObjectDataArraySize = 0;
	for(auto& item : root.items()) {
		size_t value = std::stoi(item.key().c_str());
		if(ObjectDataArraySize < value) {
			ObjectDataArraySize = value;
		}
	}

	objectDataArray_.resize(ObjectDataArraySize + 1);


	for(auto& item : root.items()) {

		size_t index               = std::stoi(item.key().c_str());
		auto   jsonPos             = item.value()["offsetPosition"];
		auto   jsonStartedT        = item.value()["startedT"];
		//auto   jsonCreateClassName = item.value()["createClassName"];

		ObjectData data = {
			.offsetPosition  = { jsonPos.at(0), jsonPos.at(1), jsonPos.at(2) },
			.startedT        = jsonStartedT,
		};

		objectDataArray_[index] = data;
	}
}



void BackgroundObjectManager::CreateBBObject(const std::string& _className, const Vec3& wPosition) {
	if(_className == "MovingLight") {
		CreateObject<MovingLight>(wPosition); 
		return; 
	}


}

void BackgroundObjectManager::SetShootingCourse(ShootingCourse* _shootingCourse) {
	pShootingCourse_ = _shootingCourse;
}

void BackgroundObjectManager::SetRailCamera(RailCamera* _railCamera) {
	pRailCamera_ = _railCamera;
}

