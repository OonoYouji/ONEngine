#include "BossAttackParticle.h"


#include "ONEngine/GraphicManager/ModelManager/ModelManager.h"

#include "Input/Input.h"

#include <GraphicManager/Drawer/Material/Material.h>
#include <ComponentManager/SpriteRenderer/SpriteRenderer.h>
#include <ComponentManager/Collider/SphereCollider.h>
#include <ComponentManager/Collider/BoxCollider.h>
#include <ComponentManager/SplinePathRenderer/SplinePathRenderer.h>
#include "Game/CustomComponents/EarthRenderer/EarthRenderer.h"
//std
#include <algorithm>
#include<numbers>
#include <limits>
#include "ImGuiManager/ImGuiManager.h"
#include"FrameManager/Time.h"
//obj
#include"Objects/Player/Player.h"
#include"Objects/Building/BuildingManager.h"
#include"Objects/Ground/Ground.h"
//function
#include"Easing/EasingFunction.h"
#include"HormingFunction/Horming.h"
#include"Math/Random.h"
#include"FrameManager/Time.h"
#undef max

void BossAttackParticle::Initialize() {

	////////////////////////////////////////////////////////////////////////////////////////////
	//  初期化
	////////////////////////////////////////////////////////////////////////////////////////////
	/*pivot_.Initialize();*/
	///////////////////////////////////////////////////////////////////////////////////////////
	// 値セット
	////////////////////////////////////////////////////////////////////////////////////////////
	pTransform_->quaternion = { 0,0,0,1 };
	pTransform_->position.z = -(Ground::groundScale_ + 1);
	///////////////////////////////////////////////////////////////////////////////////////////
	// 回転モード
	////////////////////////////////////////////////////////////////////////////////////////////	
	pTransform_->rotateOrder = QUATERNION;
	//pivot_.rotateOrder = QUATERNION;
	///////////////////////////////////////////////////////////////////////////////////////////
	//	ペアレント
	////////////////////////////////////////////////////////////////////////////////////////////	
	/*pTransform_->SetParent(&pivot_);*/
	//行列更新
	/*pivot_.UpdateMatrix();*/
	UpdateMatrix();

	const uint32_t kParticleMaxNum = 12u;

	particleDataArray_.resize(kParticleMaxNum);
	ParticleSystem* particleSystem = AddComponent<ParticleSystem>(kParticleMaxNum, "AttackEffect");


	/// パーティクルデータの初期化
	for (auto& data : particleDataArray_) {
	/*	data.pivot.Initialize();*/
		data.maxPosY = Random::Float(1.0f, 10.0f);
		data.radius = Random::Float(1.0f, 2.0f);
		data.rotateSpeed = Random::Float(5.0f, 10.0f);
		data.time = Random::Float(0.0f, 1.0f);
	}

	/// パーティクルの挙動
	particleSystem->SetParticleLifeTime(3.0f);
	particleSystem->SetEmittedParticleCount(10);
	particleSystem->SetParticleRespawnTime(0.6f);
	particleSystem->SetUseBillboard(false);

	particleSystem->SetPartilceUpdateFunction([&](Particle* particle) {
		Transform* transform = particle->GetTransform();
		ParticleData& data = particleDataArray_[particle->GetID()];

		// 回転処理
		pTransform_->rotate.z += data.rotateSpeed *Time::DeltaTime();
		data.velocity.z += (kGravity_ * Time::DeltaTime());

		//変位
		transform->position += (data.velocity / 0.0166f) * Time::DeltaTime();/// 0.0166f

		// 反発する
		if (transform->position.z > -(Ground::groundScale_)) {
			transform->position.z = -(Ground::groundScale_);
			// 反発係数により反発する
			data.velocity.z *= reboundFactor_;
			data.rotateSpeed *= reboundFactor_;
		}

	/*	data.time += Time::DeltaTime();
		transform->rotate = data.rotate;
		transform->scale = data.scale;

		transform->position = {
			std::cos(data.time * data.speed) * data.radius,
			std::sin(data.time * data.speed) * data.radius,
			particle->GetNormLifeTime() * data.maxPosY
		};
	
		transform->position = Mat4::Transform(transform->position, matRotate_);
		transform->position += GetPosition();*/
	});
}

void BossAttackParticle::Update() {

	//pivot_.UpdateMatrix();
}




void BossAttackParticle::Debug() {

}