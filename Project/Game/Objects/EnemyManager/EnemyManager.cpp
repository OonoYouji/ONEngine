#include "EnemyManager.h"

/// std
#include <format>
#include <xmemory>

/// external
#include <imgui.h>

/// user
#include "../Enemy/Enemy.h"

EnemyManager::EnemyManager() {
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

}

void EnemyManager::Update() {

}

void EnemyManager::Debug() {
	
	EmitterEdit();

}

void EnemyManager::EmitterEdit() {

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

	int emitEnemyNum = static_cast<int>(_data.config.emitEnemyNum);
	if(ImGui::DragInt(std::format("emitEnemyNum{}", pointer).c_str(), &emitEnemyNum, 1)) {
		/// 0未満にならないようにする
		_data.config.emitEnemyNum = static_cast<uint32_t>(emitEnemyNum < 0 ? 0 : emitEnemyNum);
	}

}
