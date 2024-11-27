#include "IEnemyBullet.h"

/// stl
#include <array> 

/// engine
#include "FrameManager/Time.h"
#include "GraphicManager/ModelManager/ModelManager.h"
#include "VariableManager/VariableManager.h"

/// component
#include "ComponentManager/Collider/SphereCollider.h"
#include "Game/Objects/Player/Player.h"
#include "Objects/Player/Behavior/PlayerAvoidanceBehavior.h"

#include "imgui.h"

IEnemyBullet::IEnemyBullet(BulletType type){
	CreateTag(this);

	type_ = type;
}

IEnemyBullet::~IEnemyBullet(){}

void IEnemyBullet::Initialize(){
	{// Component
		Model* model 	= ModelManager::Load(modelByBulletType[static_cast<int32_t>(type_)]);
		sphereCollider_ = AddComponent<SphereCollider>(model);
		sphereCollider_->SetRadius(2.1f);
	}
	isActive = true;
}

void IEnemyBullet::Update(){
	lifeLeftTime_ -= Time::DeltaTime();

	// lifeTime が 0 になったら 死亡
	if(lifeLeftTime_ <= 0.0f){
		isActive = false;
	}
}

void IEnemyBullet::OnCollisionEnter(BaseGameObject* const _collision){
	if(_collision->GetTag() == "Player"){
		Player* player = static_cast<Player*>(_collision);

		if(player->GetIsInvisible()){
			return;
		}


		/// ジャスト回避の時間内であれば除外
		PlayerAvoidanceBehavior* avoi = dynamic_cast<PlayerAvoidanceBehavior*>(player->GetCurrentBehavior());
		if(avoi && avoi->GetJastAvoidanceTime() > 0.0f){
			return;
		}


		player->SetHp(player->GetCurrentHP() - damage_);


		///　シェイク、ビネット
		float scale = damage_ / 250.0f;
		player->DamageEffectStart(
			2.0f,damage_,
			0.1f * scale, 10.3f * scale
		);
	}
}