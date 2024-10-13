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
	//ホーミング
	Vec3 QuaternionToEulerAngles(const Quaternion& q);
	Quaternion ToQuaternion(const Vec3& eulerAngles);



	void OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision)override;
	void GrowForTime(const float& par, const float& second);
	//getter
	bool GetIsDeath()const { return isDeath_; }
	bool GetIsSlurped()const { return isSlurp_; }
	float GetPhi() const{ return phi_; }
	float GetTheta() const{ return theta_; }
	bool GetIsCollisionTyphoon()const { return isCollisionTyphoon_; }
	Model* GetModel()const { return model_; }
	//setter
	void SetPivot(Transform pivot) { pivot_ = pivot; }//ピボット
	void SetPhi(float phi) { phi_ = phi; }//経度
	void SetTheta(float theta) { theta_ = theta; }//緯度
	void SetPivotQuaternion(Quaternion q) { pivot_.quaternion *= q; }
	//吸い込み系
	void SetDirection(float drection) { slurpDirection_ = drection; }//向き
	void SetIsSlurped(bool is) { isSlurp_ = is; }//吸われるか
	void SetSlurpPos(Vector3 pos) { slurpPos_ = pos; }//吸われる場所

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
	//死亡情報用のパラメータ
	float theta_;
	float phi_;

	//吸われるか
	bool isSlurp_;
	float slurpDirection_;
	Vector3 slurpPos_;
	
	//トルネード
	/*Tornado* pTornado_;*/
};