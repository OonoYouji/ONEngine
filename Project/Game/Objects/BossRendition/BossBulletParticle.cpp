#include "BossBulletParticle.h"


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

void BossBulletParticle::Initialize() {

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

	const uint32_t kParticleMaxNum = 15u;

	//パーティクルAddComponent
	particleDataArray_.resize(kParticleMaxNum);
	particleSystem_ = AddComponent<ParticleSystem>(kParticleMaxNum, "axis");

	/// パーティクルの挙動
	particleSystem_->SetEmittedParticleCount(10);
	particleSystem_->SetEmitterFlags(PARTICLE_EMITTER_NOTIME);

	ParticleInit();
	ParticleUpdate();

}

void BossBulletParticle::Update() {

	//pivot_.UpdateMatrix();
}


/// パーティクル初期化
void BossBulletParticle::ParticleInit() {
	/// パーティクルデータの初期化
	for (auto& data : particleDataArray_) {
		data.rotateSpeed = Random::Float(5.0f, 10.0f);/// 回転スピード
		data.transform.Initialize();	/// Transform初期化
		data.pivot.Initialize();//pivot初期化
		data.pivot = pivot_;//pivot代入
		data.transform.position = { 0,Random::Float(0,1) ,-(Ground::groundScale_ - positionOfset_) };
		data.velocity = { Random::Float(-2.0f,2.0f),Random::Float(-2.0f,2.0f),-15 };/// 速度
	}
	
}

/// <summary>
///  パーティクル更新
/// </summary>
void BossBulletParticle::ParticleUpdate() {

	particleSystem_->SetPartilceUpdateFunction([&](Particle* particle) {
		Transform* transform = particle->GetTransform();
		ParticleData& data = particleDataArray_[particle->GetID()];

		transform->SetParent(&data.pivot);

		// 回転処理
		data.transform.rotate.z += data.rotateSpeed * Time::DeltaTime();
		data.velocity.z += (kGravity_ * Time::DeltaTime());

		// 変位
		data.transform.position += (data.velocity) * Time::DeltaTime();

		// 反発する
		if (data.transform.position.z > -(Ground::groundScale_ - positionOfset_)) {
			data.transform.position.z = -(Ground::groundScale_ - positionOfset_);
			// 反発係数により反発する
			data.velocity.z *= reboundFactor_;
			data.rotateSpeed *= reboundFactor_;
			data.reflectionCount++;  // 反発カウントインクリメント
		}

		// 反発カウントが2回以上になったら止める
		if (data.reflectionCount >= 2) {
			data.velocity.x = 0.0f;
			data.velocity.z = 0.0f;
		}

		// 反発カウントが最大に達したらy軸の速度を止める
		if (data.reflectionCount >= reflectionCountMax_) {
			data.velocity.y = 0;
			data.transform.position.z = -(Ground::groundScale_ - positionOfset_);
		}

		// 回転の適用
		float rotateXAngle_ = +data.velocity.y;
		float rotateYAngle_ = -data.velocity.x;

		if (data.velocity != Vec3(0.0f, 0.0f, 0.0f)) {
			// 回転を適応
			Quaternion rotateX_ = Quaternion::MakeFromAxis({ 1.0f, 0.0f, 0.0f }, rotateXAngle_);
			Quaternion rotateY_ = Quaternion::MakeFromAxis({ 0.0f, 1.0f, 0.0f }, rotateYAngle_);

			// パーティクルの向きの決定
			/*Quaternion quaternionLocalZ = Quaternion::MakeFromAxis({ 0.0f, 0.0f, 1.0f }, std::atan2(data.velocity.x, data.velocity.y));*/

			data.pivot.quaternion *= (rotateX_ * rotateY_);  // 回転の適用と正規化
			//transform->quaternion = quaternionLocalZ.Conjugate();  // ローカルZ軸のクォータニオンの適用
		}

		// transformの更新
		transform->position = data.transform.position;
		transform->rotate = data.rotate;
		transform->scale = { 1.2f, 1.2f, 1.2f };

		});
}



void BossBulletParticle::Debug() {

}


void BossBulletParticle::SetPivot(Transform pivot) { 
	pivot_.Initialize();
	pivot_ = pivot; }