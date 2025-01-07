#include "EnemyManager.h"

/// std
#include <format>
#include <fstream>

/// external
#include <imgui.h>
#include <nlohmann/json.hpp>

/// engine
#include "FrameManager/Time.h"

/// user
#include "../Enemy/Enemy.h"
#include "../EnemyEmitter/EnemyEmitter.h"


using namespace nlohmann;


EnemyManager::EnemyManager(Player* _playerPtr) 
	: pPlayer_(_playerPtr) {
	CreateTag(this);
}

EnemyManager::~EnemyManager() {}

void EnemyManager::Initialize() {


	/// emitter dataの初期化
	sourceEmitterData_ = {
		.position = { 0.0f, 0.0f, 0.0f },
		.config   = {
			.emitEnemyNum = 5,
			.radius       = 5.0f
		}
	};

	/// jsonファイルから読み込み
	EmitterDataLoadFromJsonFile(directoryPath_, false);




}

void EnemyManager::Update() {

	for(auto itr = enemyEmitterList_.begin(); itr != enemyEmitterList_.end();) {

		Emitter* emitter = *itr;
		if(emitter->GetIsEnd()) {
			/// enemyListに追加
			enemyList_.splice(enemyList_.end(), emitter->GetEnemyList());
			emitter->Destory();
			itr = enemyEmitterList_.erase(itr);
			continue;
		}

		itr++;
	}
	
}

void EnemyManager::Debug() {

	EmitterEdit();

}

void EnemyManager::EmitterEdit() {


	if(ImGui::Button("save to json")) {
		EmitterDataSaveToJsonFile(directoryPath_, true);
	}

	ImGui::SameLine();

	if(ImGui::Button("load from json")) {
		EmitterDataLoadFromJsonFile(directoryPath_, true);
	}


	if(ImGui::TreeNode("emitter edit")) {

		/// sourceDataの編集
		EmitterDataImGuiDebug(sourceEmitterData_);

		if(ImGui::Button("add emitter")) {
			emitterDatas_.push_back(sourceEmitterData_);
		}


		/// ---------------------------------------------------
		/// すべてのEmitterDataを表示
		/// ---------------------------------------------------
		if(!emitterDatas_.empty()) { /// 空でなければ表示
			if(ImGui::BeginChild("space", ImVec2(0.0f, 360.0f), true, ImGuiWindowFlags_AlwaysVerticalScrollbar)) {

				/// すべてのEmitterDataを表示
				for(auto itr = emitterDatas_.begin(); itr != emitterDatas_.end();) {

					/// 間隔を空ける
					ImGui::Spacing();

					/// 削除ボタン
					if(ImGui::Button(std::format("remove##{:p}", reinterpret_cast<void*>(&(*itr))).c_str())) {
						itr = emitterDatas_.erase(itr);
						continue;
					}

					ImGui::SameLine();
					ImGui::Text("index : %d", std::distance(emitterDatas_.begin(), itr));

					EmitterDataImGuiDebug(*itr);

					itr++;
				}

				ImGui::EndChild();
			}
		}



		ImGui::TreePop();
	}

}

void EnemyManager::EmitterDataImGuiDebug(EmitterData& _data) {

	std::string pointer = std::format("##{:p}", reinterpret_cast<void*>(&_data));

	ImGui::DragFloat3(std::format("position{}", pointer).c_str(), &_data.position.x, 0.1f);
	ImGui::DragFloat(std::format("radius{}", pointer).c_str(), &_data.config.radius, 0.05f);
	ImGui::DragFloat(std::format("activion time{}", pointer).c_str(), &_data.config.activionTime, 0.1f);

	int emitEnemyNum = static_cast<int>(_data.config.emitEnemyNum);
	if(ImGui::DragInt(std::format("emitEnemyNum{}", pointer).c_str(), &emitEnemyNum, 1)) {
		/// 0未満にならないようにする
		_data.config.emitEnemyNum = static_cast<uint32_t>(emitEnemyNum < 0 ? 0 : emitEnemyNum);
	}
}

void EnemyManager::EmitterDataSaveToJsonFile(const std::string& _directoryPath, bool _isDrawPopupWindow) {
	/// jsonファイルに保存
	json root;
	root["emitterDatas"] = nlohmann::json::array();
	
	/// EmitterDataをjsonに変換して追加
	for(const EmitterData& data : emitterDatas_) {
		nlohmann::json j;
		j["position"] = { data.position.x, data.position.y, data.position.z };
		j["config"] = {
			{ "activionTime", data.config.activionTime },
			{ "emitEnemyNum", data.config.emitEnemyNum },
			{ "radius", data.config.radius }
		};
		root["emitterDatas"].push_back(j);
	}
	
	/// jsonファイルに書き込み
	std::ofstream ofs(_directoryPath);
	ofs << std::setw(4) << root << std::endl;

	/// 成功したらポップアップウィンドウを出す
	if(_isDrawPopupWindow) {
		if(ofs) {
			MessageBoxA(nullptr, "save success", "EnemyManager::EmitterDataSaveToJson()", MB_OK | MB_ICONINFORMATION);
		}
	}
	
}

void EnemyManager::EmitterDataLoadFromJsonFile(const std::string& _directoryPath, bool _isDrawPopupWindow) {
	/// jsonファイルを読み込み
	std::ifstream ifs(_directoryPath);
	if(!ifs) {
		MessageBoxA(nullptr, "file not found", "EnemyManager::EmitterDataLoadFromJson()", MB_OK | MB_ICONERROR);
		return;
	}

	json root;
	ifs >> root;

	/// EmitterDataを読み込み
	emitterDatas_.clear();
	for(const auto& data : root["emitterDatas"]) {
		
		EmitterData emitterData = {
			.position = {
				data["position"][0],
				data["position"][1],
				data["position"][2]
			},
			.config = {
				.activionTime = data["config"]["activionTime"],
				.emitEnemyNum = data["config"]["emitEnemyNum"],
				.radius       = data["config"]["radius"]
			}
		};

		emitterDatas_.push_back(emitterData);
	}

	/// 成功したらポップアップウィンドウを出す
	if(_isDrawPopupWindow) {
		MessageBoxA(nullptr, "load success", "EnemyManager::EmitterDataLoadFromJson()", MB_OK | MB_ICONINFORMATION);
	}


	/// emitterを再生成する
	for(auto& emitter : enemyEmitterList_) {
		emitter->Destory();
	}
	enemyEmitterList_.clear();

	for(const EmitterData& data : emitterDatas_) {
		EnemyEmitter* emitter = new EnemyEmitter(pPlayer_, this);
		emitter->Initialize();
		emitter->SetPosition(data.position);
		emitter->SetConfig(data.config);
		enemyEmitterList_.push_back(emitter);
	}

}

void EnemyManager::DestroyEnemy(Enemy* _enemy) {
	enemyList_.remove(_enemy);
	_enemy->Destroy();
}
