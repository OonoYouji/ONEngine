#include"Game/UI/TutorialUI.h"

#include "ONEngine/GraphicManager/ModelManager/ModelManager.h"

#include "Input/Input.h"
#include <ComponentManager/MeshRenderer/MeshRenderer.h>
#include <ComponentManager/Collider/BoxCollider.h>
#include "Game/CustomComponents/EarthRenderer/EarthRenderer.h"

#include "ImGuiManager/ImGuiManager.h"
#include"FrameManager/time.h"

#include"Easing/EasingFunction.h"

/// ===================================================
/// 基底チュートリアルUIクラス
/// ===================================================
void BaseTutorialUI::Initialize() {
	drawLayerId = 1;
	isClose_ = false;
	isActive = false;
	canClose_ = false;
	pTransform_->position.x = 0.2f;
	pTransform_->position.y = -3.7f;
	pTransform_->position.z = 25.6f;
	pTransform_->rotate.x = 3.25f;

	
	//BaseTutorialUI::Initialize();
};

void BaseTutorialUI::Update() {
	if (!canClose_) {
		easeT_ += Time::DeltaTime();

		if (easeT_ >= kEaseTime_) {
			easeT_ = kEaseTime_;
			canClose_ = true;
		}
		pTransform_->scale.x = EaseInCirc<float>(0.0f, 1.0f, easeT_, kEaseTime_);
	}

	if (canClose_) {
		//閉じる
		if (isClose_) {
			easeT_ -= Time::DeltaTime();
			if (easeT_ <= 0) {
				easeT_ = 0;
				isDeath_ = true;
			}
		}
		pTransform_->scale.x = EaseInCirc<float>(0.0f, 1.0f, easeT_, kEaseTime_);
	}
};

void  BaseTutorialUI::SetUIModel(const std::string& filePath) {
	Model* model = ModelManager::Load(filePath);
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel(model);
}

void BaseTutorialUI::Debug() {

};

/// ===================================================
/// 竜巻拡大UIクラス
/// ===================================================
/// 
void TutorialScaleUpUI::Initialize() {

	Model* model = ModelManager::Load("Board");
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel(model);
	BaseTutorialUI::Initialize();
};

void TutorialScaleUpUI::Update() {
	BaseTutorialUI::Update();
};

void TutorialScaleUpUI::Debug() {

};

 //===================================================
 //竜巻縮小UIクラス
 //===================================================
 
void TutorialScaleDownUI::Initialize() {

	Model* model = ModelManager::Load("Board");
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel(model);
	BaseTutorialUI::Initialize();
};

void TutorialScaleDownUI::Update() {
	BaseTutorialUI::Update();
};

void TutorialScaleDownUI::Debug() {

};


//===================================================
//巻きこめUIクラス
//===================================================
void TutorialEntrainment::Initialize() {

	Model* model = ModelManager::Load("Board");
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel(model);
	BaseTutorialUI::Initialize();
};

void TutorialEntrainment::Update() {
	BaseTutorialUI::Update();
};

void TutorialEntrainment::Debug() {

};




//===================================================
//全部巻きこめクラス
//===================================================
void TutorialEntrainmentAll::Initialize() {

	Model* model = ModelManager::Load("Board");
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel(model);
	BaseTutorialUI::Initialize();
};

void TutorialEntrainmentAll::Update() {
	BaseTutorialUI::Update();
};

void TutorialEntrainmentAll::Debug() {

};

//===================================================
//体当たりしろクラス
//===================================================

void TutorialBodyBlow::Initialize() {

	Model* model = ModelManager::Load("Board");
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel(model);
	BaseTutorialUI::Initialize();
};

void TutorialBodyBlow::Update() {
	BaseTutorialUI::Update();
};

void TutorialBodyBlow::Debug() {

};

