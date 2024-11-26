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

#include "imgui.h"

std::array<std::string,static_cast<int32_t>(BulletType::COUNT)> modelByBulletType = {
	"wrasse",
	"wrasse"
};

IEnemyBullet::IEnemyBullet(BulletType type){
	CreateTag(this);

	type_ = type;
}

IEnemyBullet::~IEnemyBullet(){
}

void IEnemyBullet::Initialize(){
	{// Component
		Model* model 	= ModelManager::Load(modelByBulletType[static_cast<int32_t>(type_)]);
		sphereCollider_ = AddComponent<SphereCollider>(model);
	}
	isAlive_ = true;
}

void IEnemyBullet::Update(){
	lifeLeftTime_ -= Time::DeltaTime();

	// lifeTime が 0 になったら 死亡
	if(lifeLeftTime_ <= 0.0f){
		isAlive_ = false;
	}
}

void IEnemyBullet::OnCollisionEnter(BaseGameObject* const _collision){
	if(_collision->GetTag() == "Player"){
		Player* player = reinterpret_cast<Player*>(_collision);
		player->SetHp(player->GetCurrentHP() - damage_);
	}
}
