#include "EnemyManager.h"

/// std
#include <iostream>
#include <fstream>
#include <string>

/// externals
#include <json.hpp>

/// engine
#include <imgui.h>

/// component
#include "ComponentManager/MeshInstancingRenderer/MeshInstancingRenderer.h"

/// objects
#include "Enemy.h"
#include "../ShootingCourse/ShootingCourse.h"
#include "../RailCamera/RailCamera.h"


using namespace nlohmann;


EnemyManager::EnemyManager() {
	CreateTag(this);
}

EnemyManager::~EnemyManager() {}

void EnemyManager::Initialize() {
	meshInstancingRenderer_ = AddComponent<MeshInstancingRenderer>(128);
	popPositionRenderer_    = AddComponent<MeshInstancingRenderer>(128);

	meshInstancingRenderer_->SetModel("EnemyStartedAnchor");
	popPositionRenderer_->SetModel("Arrow");

	LoadFile("./Resources/Parameters/EnemyManager/");
	enemyCreateDataArray_ = ioDataArray_;

}

void EnemyManager::Update() {
	CalcuationEnemyStartedAnchorPoint();
	CalcuationEnemyPopTransform();

	const std::vector<AnchorPoint>& aps = pShootingCourse_->GetAnchorPointArray();
	AnchorPoint ap{};

	float movingTime    = pRailCamera_->GetMovingTime();
	float preMovingTime = pRailCamera_->GetPreMovingTime();

	for(auto itr = enemyCreateDataArray_.begin(); itr != enemyCreateDataArray_.end();) {
		IOData& data = (*itr);

		if(movingTime > data.startedT && data.startedT > preMovingTime) {
			float t = 1.0f / static_cast<float>(aps.size()) * data.startedT;
			ap = SplinePosition(aps, t);

			CreateEnemy(
				Vec3::kOne, {}, ap.position + data.startOffset,
				data.hp, data.startedT
			);

			itr = enemyCreateDataArray_.erase(itr);
		} else {
			itr++;
		}
	}

}

void EnemyManager::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::SeparatorText("com");

		if(ImGui::Button("save")) {
			SaveFile("./Resources/Parameters/EnemyManager/");
		}


		ImGui::SeparatorText("io data");

		if(ImGui::Button("add")) {
			ioDataArray_.push_back(IOData(
				{ 0, 0, 0 },
				{ 0, 0, 1 },
				1.0f, 10.0f, 1.0f
			));
		}


		ImGui::BeginChild("io data array", ImVec2(0, 360.0f), true, ImGuiWindowFlags_HorizontalScrollbar);
		size_t index = 0;
		for(auto& ioData : ioDataArray_) {
			ImGui::DragFloat3((std::string("start offset##")   + std::to_string(index)).c_str(), &ioData.startOffset.x, 0.25f);
			
			if(ImGui::DragFloat3((std::string("move direction##") + std::to_string(index)).c_str(), &ioData.direction.x, 0.25f)) {
				ioData.direction = ioData.direction.Normalize();
			}
			ImGui::DragFloat((std::string("move speed##")      + std::to_string(index)).c_str(), &ioData.speed,         0.25f);

			ImGui::DragFloat((std::string("hp##")              + std::to_string(index)).c_str(), &ioData.hp,            0.25f);
			ImGui::DragFloat((std::string("startedT##")        + std::to_string(index)).c_str(), &ioData.startedT,      0.25f);

			ImGui::Spacing();
			++index;
		}
		ImGui::EndChild();


		ImGui::TreePop();
	}


}

void EnemyManager::SetShootingCourse(ShootingCourse* _shootingCourse) {
	pShootingCourse_ = _shootingCourse;
}

void EnemyManager::SetRailCamera(RailCamera* _railCamera) {
	pRailCamera_ = _railCamera;
}

void EnemyManager::CreateEnemy(
	const Vec3& _scale,
	const Vec3& _rotate,
	const Vec3& _position,
	float _updateStartT,
	float _hp) const {

	Enemy* enemy = new Enemy();
	enemy->Initialize();

	enemy->SetScale(_scale);
	enemy->SetPosition(_position);
	enemy->SetRotate(_rotate);

	enemy->hp_           = _hp;
	enemy->updateStartT_ = _updateStartT;
}


void EnemyManager::SaveFile(const std::string& filePath) {
	json root = json::object();
	for(size_t i = 0; i < ioDataArray_.size(); ++i) {
		auto& item = root[std::to_string(i)];

		IOData& data = ioDataArray_[i];
		item["startOffset"] = json::array({ data.startOffset.x, data.startOffset.y, data.startOffset.z });
		item["direction"]   = json::array({ data.direction.x, data.direction.y, data.direction.z });
		item["speed"]       = data.speed;
		item["startedT"]    = data.startedT;
		item["hp"]          = data.hp;
	}

	///- ディレクトリがなければ作成する
	std::filesystem::path dir(filePath);
	if(!std::filesystem::exists(dir)) {
		std::filesystem::create_directories(dir);
	}

	///- File open
	std::string path = filePath + "EnemyManager.json";
	std::ofstream ofs;
	ofs.open(path);

	if(ofs.fail()) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "EnemyManager", 0);
		assert(false);
		return;
	}

	///- ファイルにjson文字列を書き込む(インデント幅4)
	ofs << std::setw(4) << root << std::endl;
	ofs.close();
}

void EnemyManager::LoadFile(const std::string& filePath) {
	///- ファイルを開く
	std::string path = filePath + "EnemyManager.json";
	std::ifstream ifs;
	ifs.open(path);

	///- 開けなければメッセージを出す
	if(!ifs.is_open()) {
		std::string message = "File could not be opened.";
		MessageBoxA(nullptr, message.c_str(), "EnemyManager", 0);
		assert(false);
		return;
	}

	///- json文字列からjsonのデータ構造に展開
	json root;
	ifs >> root;
	ifs.close();


	for(auto& item : root.items()) {

		auto jsonPos       = item.value()["startOffset"];
		auto jsonDirection = item.value()["direction"];
		auto jsonSpeed     = item.value()["speed"];

		auto jsonStartedT  = item.value()["startedT"];
		auto jsonHP        = item.value()["hp"];

		IOData ioData {
			.startOffset = { jsonPos.at(0), jsonPos.at(1), jsonPos.at(2) },
			.direction   = { jsonDirection.at(0), jsonDirection.at(1), jsonDirection.at(2) },
			.speed       = jsonSpeed,
			.startedT    = jsonStartedT,
			.hp          = jsonHP
		};

		ioDataArray_.push_back(ioData);
	}
}

void EnemyManager::CalcuationEnemyStartedAnchorPoint() {

	const std::vector<AnchorPoint>& anchorPoints = pShootingCourse_->GetAnchorPointArray();

	AnchorPoint ap{};
	for(size_t i = 0; i < ioDataArray_.size(); ++i) {
		ap = SplinePosition(anchorPoints, 1.0f / static_cast<float>(anchorPoints.size()) * ioDataArray_[i].startedT);

		if(i >= startedTTransforms_.size()) {

			Transform transform;
			transform.position = ap.position;
			transform.UpdateMatrix();
			startedTTransforms_.push_back(transform);

		} else {

			startedTTransforms_[i].position = ap.position;
			startedTTransforms_[i].UpdateMatrix();

		}
	}


	/// mesh instancing rendererに渡す
	meshInstancingRenderer_->ResetTransformArray();
	for(auto& transform : startedTTransforms_) {
		meshInstancingRenderer_->AddTransform(&transform);
	}

}

void EnemyManager::CalcuationEnemyPopTransform() {
	
	enemyPopTransforms_.resize(ioDataArray_.size());

	for(size_t i = 0; i < ioDataArray_.size(); ++i) {
		Transform& startAnchorTransform = startedTTransforms_[i];
		IOData&    ioData               = ioDataArray_[i];

		Transform& transform  = enemyPopTransforms_[i];
		transform.rotateOrder = QUATERNION;

		transform.position    = startAnchorTransform.position + ioData.startOffset;
		transform.quaternion  = Quaternion::LockAt({}, ioData.direction);

		transform.Update();
	}

	/// mesh instancing rendererに渡す
	popPositionRenderer_->ResetTransformArray();
	for(auto& transform : enemyPopTransforms_) {
		popPositionRenderer_->AddTransform(&transform);
	}
}

