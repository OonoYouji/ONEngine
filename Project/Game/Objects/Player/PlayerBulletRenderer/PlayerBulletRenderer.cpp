#include "PlayerBulletRenderer.h"

/// engine
#include "ComponentManager/MeshInstancingRenderer/MeshInstancingRenderer.h"

/// game
#include "../PlayerBullet/PlayerBullet.h"

PlayerBulletRenderer::PlayerBulletRenderer() {
	CreateTag(this);
}

PlayerBulletRenderer::~PlayerBulletRenderer() {}

void PlayerBulletRenderer::Initialize() {
	instancingRenderer_ = AddComponent<MeshInstancingRenderer>(128);
	instancingRenderer_->SetModel("PlayerBullet");

}

void PlayerBulletRenderer::Update() {

	/// 死んでいるオブジェクトをリストから削除する
	std::erase_if(bullets_, [](PlayerBullet* _bullet) {
		if(_bullet->GetIsAlive()) {
			return false;
		}

		_bullet->Destory();
		return true;
	});


	/// 描画するtransformを追加
	instancingRenderer_->ResetTransformArray();
	for(auto& bullet : bullets_) {
		instancingRenderer_->AddTransform(bullet->GetTransform());
	}
}

void PlayerBulletRenderer::PushBackBullet(PlayerBullet* _bullet) {
	bullets_.push_back(_bullet);
}

