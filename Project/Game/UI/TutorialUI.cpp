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
	isClose_ = false;
	isActive = false;
	pTransform_->position.x = -0.3f;
	pTransform_->position.y = -3.8f;
	pTransform_->position.z = -16.5f;
	pTransform_->rotate.x = 3.8f;
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