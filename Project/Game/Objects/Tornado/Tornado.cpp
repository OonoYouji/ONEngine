#define NOMINMAX
#include "Tornado.h"

/// std
#include <numbers>

/// engine
#include "ImGuiManager/ImGuiManager.h"
#include "FrameManager/Time.h"
#include <Input/Input.h>

/// components
#include <ComponentManager/MeshRenderer/MeshRenderer.h>

/// objects
#include "Objects/Player/Player.h"


void Tornado::Initialize() {


	/*/// mesh renderer 初期化
	MeshRenderer* mrArray[]{
		AddComponent<MeshRenderer>(),
		AddComponent<MeshRenderer>(),
		AddComponent<MeshRenderer>()
	};

	mrArray[0]->SetModel("TornadoRing1");
	mrArray[1]->SetModel("TornadoRing2");
	mrArray[2]->SetModel("TornadoRing3");*/


	/// transform initialize
	pTransform_->rotateOrder = QUATERNION;

	quaternionLocalX_ = Quaternion::MakeFromAxis(Vec3::kRight, -std::numbers::pi_v<float> / 2.0f);
	quaternionLocalY_ = Quaternion::MakeFromAxis(Vec3::kUp, 0.0f);
	/// action param initialize
	eacSpeed_    = 0.7f;

	scaleScaler_ = 1.0f;
	minScale_    = 1.0f;
	maxScale_    = 3.0f;


	/// ring array initializing
	ringArray_.resize(3);
	for(auto& ring : ringArray_) {
		ring = new Ring;
		ring->Initialize();
		ring->SetParent(pTransform_);
	}

}

void Tornado::Update() {

	if(Input::PressKey(KeyCode::Space)) {

		scaleScaler_ = std::min(scaleScaler_ + (eacSpeed_ * Time::DeltaTime()), maxScale_);
	} else {

		scaleScaler_ = std::max(scaleScaler_ - (eacSpeed_ * Time::DeltaTime()), minScale_);
	}

	localYAngle_  += Time::DeltaTime() * zRotateSpeed_;

	quaternionLocalY_       = Quaternion::MakeFromAxis(Vec3::kUp,    localYAngle_);

	pTransform_->quaternion = quaternionLocalX_;
	pTransform_->scale      = Vec3::kOne * scaleScaler_;

}



void Tornado::Debug() {

	if(ImGui::TreeNodeEx("this param", ImGuiTreeNodeFlags_DefaultOpen)) {

		Vec3 wPos = GetPosition();
		ImGui::DragFloat3("world position", &wPos.x, 0.0f);

		ImGui::Separator();

		ImGui::DragFloat4("quaternion local x", &quaternionLocalX_.x, 0.0f);
		ImGui::DragFloat4("quaternion local y", &quaternionLocalX_.y, 0.0f);

		ImGui::TreePop();
	}

	/// this debug
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {
		
		ImGui::DragFloat("eac speed", &eacSpeed_, 0.01f);
		ImGui::DragFloat("z rotate speed", &zRotateSpeed_, 0.5f);

		ImGui::Separator();

		ImGui::DragFloat("scale scaler", &scaleScaler_, 0.01f);
		ImGui::DragFloat("min scale", &minScale_, 0.01f);
		ImGui::DragFloat("max scale", &maxScale_, 0.01f);
		
		ImGui::Separator();

		ImGui::DragFloat("local y angle", &localYAngle_, 0.1f);
		
		

		ImGui::TreePop();
	}


	/// player debug
	if(ImGui::TreeNodeEx("player", ImGuiTreeNodeFlags_DefaultOpen)) {

		void* pointer = reinterpret_cast<void*>(pPlayer_);
		ImGui::DragInt("address", reinterpret_cast<int*>(&pointer));

		ImGui::TreePop();
	}
}

void Tornado::SetPlayer(Player* _player) {
	pPlayer_ = _player;

	SetParent(pPlayer_->GetbaseTransform());
	
}


/// ===================================================
/// ring
/// ===================================================

int Ring::sInstanceCount_ = 0;

Ring::Ring() {
	CreateTag(this); 
	id_ = sInstanceCount_++;
}

void Ring::Initialize() {
	auto mr = AddComponent<MeshRenderer>();
	mr->SetModel(std::string("TornadoRing") + std::to_string(id_ + 1));
}

void Ring::Update() {
	pTransform_->rotate.y += Time::DeltaTime() * rotateSpeed_;
}

void Ring::Debug() {
	if (ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat("rotate speed", &rotateSpeed_, 0.5f);

		ImGui::TreePop();
	}
}
