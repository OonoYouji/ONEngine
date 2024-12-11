#include "PlayerMotionDebugRenderer.h"

/// externals
#include <json.hpp>

/// std
#include <format>
#include <numbers>

/// engine
#include "ComponentManager/MeshInstancingRenderer/MeshInstancingRenderer.h"
#include "ComponentManager/SplinePathRenderer/SplinePathRenderer.h"

/// game
#include "CustomMath/Json/CreateJsonFile.h"
#include "../Player.h"

using namespace nlohmann;


PlayerMotionDebugRenderer::PlayerMotionDebugRenderer(Player* _player) 
	: pPlayer_(_player) {
	CreateTag(this);
}

PlayerMotionDebugRenderer::~PlayerMotionDebugRenderer() {}

void PlayerMotionDebugRenderer::Initialize() {

	splinePathRenderer_ = AddComponent<SplinePathRenderer>(6);
	meshInstancingRenderer_ = AddComponent<MeshInstancingRenderer>(32);
	meshInstancingRenderer_->SetModel("Sphere");


	pBehaviorManage_ = pPlayer_->GetBehaviorManager();
}

void PlayerMotionDebugRenderer::Update() {

	BaseMotion* motion = pBehaviorManage_->GetCurrentMotion();

	splinePathRenderer_->SetAnchorPointArray({});
	for(auto& keyframe : motion->keyframes_) {
		splinePathRenderer_->AddAnchorPoint(keyframe.position);
	}
}

void PlayerMotionDebugRenderer::Debug() {
	SelectEditMotion();

	//AddMotion(tmp.get());
	MotionEdit(pBehaviorManage_->GetMotionKey(), pBehaviorManage_->GetCurrentMotion());
}

void PlayerMotionDebugRenderer::SelectEditMotion() {}

void PlayerMotionDebugRenderer::MotionEdit(const std::string& _key, BaseMotion* _motion) {
	if(ImGui::TreeNode("motion edit")) {

		if(ImGui::Button("save json")) {
			SaveMotionToJson("test", _motion);
		}

		/// 再生する
		if(ImGui::Button("play")) {
			_motion->Start();
		}

		ImGui::Separator();


		ImGui::DragFloat("current time", &_motion->currentTime_);
		ImGui::DragFloat3("current keyfrarme position", &_motion->currentKeyframe_.position.x, 0.0f);
		ImGui::DragFloat3("current keyfrarme rotate", &_motion->currentKeyframe_.rotate.x, 0.0f);
		ImGui::DragFloat3("current keyfrarme scale", &_motion->currentKeyframe_.scale.x, 0.0f);

		ImGui::Spacing();

		ImGui::DragFloat("max time", &_motion->maxTime_, 0.025f);

		ImGui::Spacing();

		if(ImGui::Button("add keyframe")) {
			_motion->keyframes_.push_back(_motion->keyframes_.back());
		}

		KeyframeEdit(_motion->keyframes_);

		ImGui::TreePop();
	}

}

void PlayerMotionDebugRenderer::KeyframeEdit(std::vector<MotionKeyframe>& _keyframes) {
	if(ImGui::TreeNode("keyframes")) {

		size_t index = 0;

		for(auto itr = _keyframes.begin(); itr != _keyframes.end(); index++) {
			MotionKeyframe& keyframe = (*itr);
			void* pointer = reinterpret_cast<void*>(&keyframe);

			ImGui::Text("index(%d) ==============================", index);

			ImGui::DragFloat(std::format("time##{:p}", pointer).c_str(), &keyframe.time, 0.1f);

			ImGui::Spacing();

			ImGui::DragFloat3(std::format("position##{:p}", pointer).c_str(), &keyframe.position.x, 0.1f);
			ImGui::DragFloat3(std::format("rotate##{:p}", pointer).c_str(), &keyframe.rotate.x, std::numbers::pi_v<float> *0.1f);
			ImGui::DragFloat3(std::format("scale##{:p}", pointer).c_str(), &keyframe.scale.x, 0.1f);

			if(ImGui::Button(std::format("remove keyframe##{:p}", pointer).c_str())) {

				/// keyframeは4以上じゃなければ削除できない
				if(_keyframes.size() > 4) {
					itr = _keyframes.erase(itr);
				}
			} else {
				itr++;
			}

			ImGui::Spacing();
			ImGui::Spacing();
		}

		ImGui::TreePop();
	}

}

void PlayerMotionDebugRenderer::SaveMotionToJson(const std::string& _fileName, BaseMotion* _motion) {
	json root = json::object();

	for(size_t i = 0; i < _motion->keyframes_.size(); ++i) {
		MotionKeyframe& keyframe = _motion->keyframes_[i];
		json& item = root[std::to_string(i)];

		item["time"] = keyframe.time;
		item["position"] = json::array({ keyframe.position.x, keyframe.position.y, keyframe.position.z });
		item["rotate"] = json::array({ keyframe.rotate.x, keyframe.rotate.y, keyframe.rotate.z });
		item["scale"] = json::array({ keyframe.scale.x, keyframe.scale.y, keyframe.scale.z });
	}


	CreateJsonFile("./Resources/Parameters/Player/", _fileName, root);
}

