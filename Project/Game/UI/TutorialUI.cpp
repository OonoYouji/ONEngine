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
	canClose_ = false;
	pTransform_->scale = {0,1,1};
	pTransform_->position.y = -1.4f;
	pTransform_->rotate.x = 0.0f;

	
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
	
	/// add components
	SpriteRenderer* spriteRenderer = AddComponent<SpriteRenderer>();
	spriteRenderer->SetTexture("uvChecker");
	
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

	/// add components
	SpriteRenderer* spriteRenderer = AddComponent<SpriteRenderer>();
	spriteRenderer->SetTexture("uvChecker");
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

	SpriteRenderer* spriteRenderer = AddComponent<SpriteRenderer>();
	spriteRenderer->SetTexture("uvChecker");
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

	SpriteRenderer* spriteRenderer = AddComponent<SpriteRenderer>();
	spriteRenderer->SetTexture("uvChecker");
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

	SpriteRenderer* spriteRenderer = AddComponent<SpriteRenderer>();
	spriteRenderer->SetTexture("uvChecker");
	BaseTutorialUI::Initialize();
};

void TutorialBodyBlow::Update() {
	BaseTutorialUI::Update();
};

void TutorialBodyBlow::Debug() {

};

