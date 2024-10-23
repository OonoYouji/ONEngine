#define NOMINMAX
#include "PlayerHP.h"

/// engine
#include <imgui.h>
#include "GraphicManager/ModelManager/ModelManager.h"
#include "GraphicManager/TextureManager/TextureManager.h"
#include "FrameManager/Time.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

/// objects
#include "Objects/Player/Player.h"


PlayerHP::PlayerHP() {
	CreateTag(this);
}

PlayerHP::~PlayerHP() {}

void PlayerHP::Initialize() {

	TextureManager::GetInstance()->Load("playerHpNum", "playerHpNum.png");
	model_ = ModelManager::Load("playerHp");

	MeshRenderer* meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel(model_);
	
	numberRenderer_ = AddComponent<MeshRenderer>();
	numberRenderer_->SetModel(model_);
	numberRenderer_->SetMaterial("playerHpNum");

	std::vector<Material>& materials = model_->GetMaterials();
	materials.front().SetPosition(gaugeUVPosition_);
	materials.front().SetIsLighting(false);

	numberUVPosition_ = { 0.12f, 0.03f };
	numberUVScale_ = { 3.0f, 0.6f };

	uvPosYArray_ = {
		0.03f,  0.185f, 0.345f,
		0.505f, 0.665f, 0.825f
	};


	animationTime_    = 0.0f;
	maxAnimationTime_ = 0.6f;

	pTransform_->position = {
		-1.9f, -0.9f, 5.5f
	};

}

void PlayerHP::Update() {

	animationTime_ += Time::DeltaTime();
	float lerpT = std::min(animationTime_ / maxAnimationTime_, 1.0f);

	/// number uv position y 
	numberUVPosition_.y = std::lerp(
		uvPosYArray_[currentHp_], uvPosYArray_[nextHp_],
		lerpT
	);

	float gaugeLerpT = static_cast<float>(nextHp_) / 5.0f;
	/// 0.5 ~ 0.25
	gaugeUVPosition_.y = std::lerp(
		0.5f, 0.25f,
		(nextHp_ / 5.0f) * gaugeLerpT
	);

	if(lerpT == 1.0f) {
		isStart_ = false;
	}



	/// マテリアルのセット

	std::vector<Material>& materials = model_->GetMaterials();
	materials.front().SetPosition(gaugeUVPosition_);
	materials.front().UpdateMatrix();

	Material* material = numberRenderer_->GetMaterial();
	material->SetPosition(numberUVPosition_);
	material->SetScale(numberUVScale_);
	material->SetIsLighting(false);
	material->UpdateMatrix();

}

void PlayerHP::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat2("gauge uv position",  &gaugeUVPosition_.x,  0.01f);

		ImGui::Spacing();

		ImGui::DragFloat2("number uv position", &numberUVPosition_.x, 0.005f);
		ImGui::DragFloat2("number uv scale",    &numberUVScale_.x,    0.01f);

		ImGui::Spacing();

		ImGui::SliderInt("current hp", &currentHp_, 0, 5);
		ImGui::SliderInt("next hp", &nextHp_, 0, 5);
		
		ImGui::SeparatorText("animation");
		ImGui::DragFloat("time",     &animationTime_,    0.2f);
		ImGui::DragFloat("max time", &maxAnimationTime_, 0.2f);

		if(ImGui::Button("reset animation")) {
			ResetAnimationTime();
		}
		
		ImGui::TreePop();
	}
}

void PlayerHP::ResetAnimationTime() {
	animationTime_ = 0.0f;
}

void PlayerHP::SetHP(int hp) {
	if(hp != currentHp_) {
		ResetAnimationTime();

		currentHp_ = nextHp_;
		nextHp_ = hp;
	}
}
