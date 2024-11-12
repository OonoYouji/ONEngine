#define NOMINMAX
#include "Boss.h"

/// engine
#include "GraphicManager/ModelManager/ModelManager.h"
#include "FrameManager/Time.h"

#include "Math/Easing.h"

/// component
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "ComponentManager/Collider/SphereCollider.h"

/// objects
#include "Objects/RailCamera/RailCamera.h"

Boss::Boss() {
	CreateTag(this);
}

Boss::~Boss() {}

void Boss::Initialize() {

	Model* model = ModelManager::Load("Sphere");

	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("Boss");
	meshRenderer_->SetMaterial("white2x2.png");
	meshRenderer_->SetColor(Vec4::kWhite);

	SphereCollider* sphereCollider = AddComponent<SphereCollider>(model);


	pTransform_->position = { 91.0f, 24.0f, -4.3f };

}

void Boss::Update() {
	meshRenderer_->SetColor(Vec4::kWhite);

	if(!isBattelStarted_) {

		/// ボス戦開始
		if(pRailCamera_->GetMovingTime() > 27.0f) {
			isBattelStarted_ = true;
			pRailCamera_->isActive = false;
		}
	} else {
		battelTime_ -= Time::DeltaTime();

		pTransform_->position.z = -5.0f + std::max(std::sin(battelTime_ * 2.0f), 0.0f) * -5.0f;
		pTransform_->position.x = 91.0f + Ease::InOut::Elastic(std::sin(battelTime_))* -2.0f;


		/// バトル終了
		if(battelTime_ < 0.0f) {
			pRailCamera_->isActive = true;
			Destory();
		}

	}
}

void Boss::OnCollisionEnter(BaseGameObject* const _collision) {
	if(_collision->GetTag() == "PlayerBullet") {
		meshRenderer_->SetColor(Vec4::kRed);
	}
}

void Boss::EndBattel() { 
	battelTime_ = false;
	pRailCamera_->isActive = true;
	Destory();
}

void Boss::SetRailCamera(RailCamera* _railCamera) {
	pRailCamera_ = _railCamera;
}

void Boss::SubHP(float _value) {
	hp_ -= _value;
}

