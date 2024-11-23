#include "IEnemyBullet.h"

/// stl
#include <array> 

/// engine
#include "FrameManager/Time.h"
#include "GraphicManager/ModelManager/ModelManager.h"
#include "VariableManager/VariableManager.h"

/// component
#include "ComponentManager/Collider/SphereCollider.h"

#include "imgui.h"

std::array<std::string,static_cast<int32_t>(BulletType::COUNT)> modelByBulletType = {
	"wrasse",
	"Sphere"
};

IEnemyBullet::IEnemyBullet(BulletType type){
	CreateTag(this);

	type_ = type;
}

IEnemyBullet::~IEnemyBullet(){}

void IEnemyBullet::Initialize(){
	{// Component
		Model* model 	= ModelManager::Load(modelByBulletType[static_cast<int32_t>(type_)]);
		sphereCollider_ = AddComponent<SphereCollider>(model);
		render_ 		= AddComponent<AnimationRenderer>(modelByBulletType[static_cast<int32_t>(type_)]);
		pTransform_->scale = {3.0f,3.0f,3.0f};
	}
}

void IEnemyBullet::Update(){
	lifeLeftTime_ -= Time::DeltaTime();

	// lifeTime が 0 になったら 死亡
	if(lifeLeftTime_ <= 0.0f){
		Destory();
	}
}