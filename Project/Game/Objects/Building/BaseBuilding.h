#pragma once

#include "GameObjectManager/GameObjectManager.h"
#include <ComponentManager/Transform/Transform.h>
#include <ComponentManager/AudioSource/AudioSource.h>
#include <ComponentManager/SpriteRenderer/SpriteRenderer.h>
#include<optional>
//obj
#include"Objects/Tornado/Tornado.h"
#include"Objects/Player/Player.h"


class BaseBuilding : public BaseGameObject {
public:

	BaseBuilding() { CreateTag(this); }
	~BaseBuilding() {}

	virtual	void Initialize()override;
	virtual	void Update() override;
	virtual void Debug() override;
	//セットトルネード
	/*void SetTornado(Tornado* tornado);*/


	void OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision)override;
	void GrowForTime(const float& par, const float& second);
	//getter
	bool GetIsDeath()const { return isDeath_; }
	bool GetIsCollisionTyphoon()const { return isCollisionTyphoon_; }
	Model* GetModel()const { return model_; }
	//setter
	void SetPivot(Transform pivot) { pivot_ = pivot; }
	void SetPivotQuaternion(Quaternion q) { pivot_.quaternion *= q; }

private:
	Model* model_=nullptr;
	//死んだか
	bool isDeath_;
	//竜巻と衝突したか
	bool isCollisionTyphoon_;
	
	//スケールMAX
	float scaleMax_;
	//スケール上昇割合
	float scalePar_;
	//成長タイム
	float growTime_;
	// //ピボット
	Transform pivot_;
	AudioSource* audioSource_ = nullptr;
	//トルネード
	/*Tornado* pTornado_;*/
};