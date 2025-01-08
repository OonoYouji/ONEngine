#include "Explostion.h"

/// engine
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "Objects/Camera/Manager/CameraManager.h"
#include "Objects/Camera/GameCamera.h"
#include "FrameManager/Time.h"


Explostion::Explostion() {
	CreateTag(this);
}

Explostion::~Explostion() {}

void Explostion::Initialize() {

	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("Explosion");
	meshRenderer_->SetMaterial("Explosion.png");
	meshRenderer_->SetColor(Vec4(Vec3::kOne * 0.75f, 0.5f));

	lifeTime_ = 0.2f;

	/// TODO: 音を流す
	particle_ = new ExplostionParticle();
	particle_->Initialize();
	particle_->SetParent(pTransform_);

}

void Explostion::Update() {

	/// cameraを取得
	BaseCamera* camera = CameraManager::GetInstance()->GetMainCamera();
	if(camera->GetTag() == "GameCamera") {
		pGameCamera_ = static_cast<GameCamera*>(camera);
	}

	/// ビルボード処理
	if(pGameCamera_) {
		pTransform_->rotate = pGameCamera_->GetTransform()->rotate;
	}

	currentTime_ += Time::DeltaTime();

	if(currentTime_ < lifeTime_ * 0.5f) {
		pTransform_->scale = Vec3::Lerp(
			Vec3::kOne, Vec3::kOne * 5.0f,
			currentTime_ / (lifeTime_ * 0.5f)
		);
	}

	if(currentTime_ > lifeTime_) {
		Destory();
		particle_->Destory();
	}


}


ExplostionParticle::ExplostionParticle() {
	CreateTag(this);
}

ExplostionParticle::~ExplostionParticle() {}

void ExplostionParticle::Initialize() {
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("ExplosionParticle");
	meshRenderer_->SetMaterial("white2x2.png");
	meshRenderer_->SetColor(Vec4(0.25f, 0.75f, 0.25f, 0.5f));
}

void ExplostionParticle::Update() {
	pTransform_->rotate += Vec3(0.1f, 0.2f, 0.0f);
}

