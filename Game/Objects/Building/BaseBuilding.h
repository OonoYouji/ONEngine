#pragma once

#include <GameObjectManager.h>
#include <Component/Transform/Transform.h>
#include <Component/AudioSource/AudioSource.h>
#include <Component/SpriteRenderer/SpriteRenderer.h>
#include<optional>

#include"Player/Player.h"

class BaseBuilding : public BaseGameObject {
public:

	BaseBuilding() { CreateTag(this); }
	~BaseBuilding() {}

	virtual	void Initialize()override;
	virtual	void Update() override;

	virtual void Debug() override;

	//振る舞い関数
	void RootInit();
	void RootUpdate();
	void ParentInit(Player* player);
	void ParentUpdate();
	void BehaviorManagement(Player* player);

	//getter
	bool GetIsDeath()const { return isDeath_; }
	bool GetIsCollisionTyphoon()const { return isCollisionTyphoon_; }
	//setter
	void SetPivot(Transform pivot) { pivot_ = pivot; }
	void SetPivotQuaternion(Quaternion q) { pivot_.quaternion *= q; }
	Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle);
private:
	enum class Behavior {
		kRoot,
		kParent,	
	};

private:
	//// ふるまい
	Behavior behavior_;
	//振る舞いリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;
	//死んだか
	bool isDeath_;
	//竜巻と衝突したか
	bool isCollisionTyphoon_=true;
	//
	float theta_;
	//
	float phi_;
	// //ピボット
	Transform pivot_;
	AudioSource* audioSource_ = nullptr;
};