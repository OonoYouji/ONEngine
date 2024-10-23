#pragma once

/// std
#include <optional>
#include <array>

/// engine
#include "GameObjectManager/GameObjectManager.h"
#include <ComponentManager/Transform/Transform.h>
#include <ComponentManager/AudioSource/AudioSource.h>
#include <ComponentManager/SpriteRenderer/SpriteRenderer.h>

/// objects
#include "Objects/Tornado/Tornado.h"
#include "Objects/Player/Player.h"


/// ===================================================
/// 建物の大きさを表す enum
/// ===================================================
enum BUILDING_SCALE {
	BUILDING_SCALE_SMALL,
	BUILDING_SCALE_NORMAL,
	BUILDING_SCALE_BIG,
	BUILDING_SCALE_COUNT /// 要素の数を数える用
};


/// ===================================================
/// 建物の基底クラス
/// ===================================================
class BaseBuilding : public BaseGameObject {
public:

	BaseBuilding() { CreateTag(this); }
	~BaseBuilding() {}

	/// ===================================================
	/// public : methods 
	/// ===================================================

	virtual	void Initialize()override;
	virtual	void Update() override;
	virtual void Debug() override;
	
	void OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision)override;

	void GrowForTime(const float& par, const float& second);

	/// <summary>
	/// ぴょこぴょこするアニメーション
	/// </summary>
	void Animation();


	/// <summary>
	/// hit pointの減少、衝突時間による減少量
	/// </summary>
	void SubHP(float _subValue);

	float GetHP() const;


	/// ===================================================
	/// public : accessor
	/// ===================================================

	//getter
	bool GetIsInTornado() const { return isInTornado_; }
	bool GetIsTaken() const { return isTaken_; }
	bool GetIsBreak() const { return isBreak_; }
	bool GetIsSlurped() const { return isSlurp_; }
	bool GetIsCollisionTyphoon() const { return isCollisionTyphoon_; }
	Model* GetModel() const { return modelArray_[currentScaleIndex_]; }
	

	int GetCurrentScaleIndex() const { return currentScaleIndex_; }

	const std::pair<float, float>& GetPos() const { return pos_; }
	//setter
	void SetPivot(Transform pivot) { pivot_ = pivot; }//ピボット
	void SetBoss(Boss* boss);
	//void SetPhi(float phi) { phi_ = phi; }//経度
	void SetPos(const std::pair<float,float>& pos) { pos_ = pos; }//緯度
	void SetPivotQuaternion(Quaternion q) { pivot_.quaternion *= q; }
	//吸い込み系
	void SetIsSlurped(bool is) { isSlurp_ = is; }//吸われるか
	void SetSlurpPos(Vector3 pos) { slurpPos_ = pos; }//吸われる場所

	void SetShake(const Vec3& _shaleValue);

	/// <summary>
	/// 竜巻に吸い込まれたかどうかのフラグ
	/// </summary>
	/// <param name="_isInTornado"> : 竜巻に巻き込まれたかのフラグ : true, false </param>
	void SetIsInTornado(bool _isInTornado);

private:
	Boss* pBoss_ = nullptr;
	//死んだか
	bool isInTornado_;
	//竜巻と衝突したか
	bool isCollisionTyphoon_;

	float slurpSpeed_;
	
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
	std::pair<float, float> pos_;

	//吸われるか
	bool isSlurp_;
	bool isTaken_;
	bool isBreak_;
	Vector3 slurpPos_;
	
	//建物を浮かせるイージング
	float floatBuildingEaseTime_;
	const float floatBuildingEaseTimeMax_=1.0f;
	const float buildingSartZ = -10.8f;

	

	/// ---------------------------------------------------
	/// state parameter
	/// ---------------------------------------------------

	float hp_ = 0.1f; /// 最小時に一瞬で回収できるくらいhp

	

	/// ---------------------------------------------------
	/// earth shadow
	/// ---------------------------------------------------

	class EarthRenderer* earthRenderer_ = nullptr;
	float shadowRaidus_ = 2.0f;

	std::array<float, BUILDING_SCALE_COUNT> shadowRadiusArray_;
	std::array<Vec4, BUILDING_SCALE_COUNT>  shadowColorArray_;



	/// ---------------------------------------------------
	/// animation
	/// ---------------------------------------------------

	float animationTime_  = 0.0f;
	float animationSpeed_ = 4.0f;



	/// ---------------------------------------------------
	/// model
	/// ---------------------------------------------------

	std::vector<Model*> modelArray_; /// 大中小のモデル配列
	int currentScaleIndex_ = 0;

	std::array<float, BUILDING_SCALE_COUNT> buildingScaleArray_;
	std::array<float, BUILDING_SCALE_COUNT> nextScalingTimeArray_;

	/// ---------------------------------------------------
	/// effect
	/// ---------------------------------------------------

	class ParticleSystem* particleSystem_ = nullptr;

	Vec3 shake_;
	Vec3 offsetPosition_;

};