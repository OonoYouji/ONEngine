#pragma once

/// std
#include <vector>

#include "GameObjectManager/GameObjectManager.h"

/// ===================================================
/// 前方宣言
/// ===================================================
class Player;
class BuildingManager;
class Wind;

/// ===================================================
/// 竜巻を表現するクラス
/// ===================================================
class Tornado final : public BaseGameObject {

	struct WindAnimationData {
		float time = 0.0f;
		float speed = 1.0f;
	};

	struct ParticleData {
		float time = 0.0f;
		float speed = 1.0f;
		float radius = 1.0f;
		float maxPosY = 0.0f;
		Vec3 rotate{};
		Vec3 scale{};
	};

	struct WindData {
		float time;
		float speed;
		float radius;
		float height;
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	Tornado() { CreateTag(this); }
	~Tornado() {}
	void OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision);

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	float GetScaleScaler()const { return scaleScaler_; }
	float GetMaxScale()const { return maxScale_; }
	float GetMinScale()const { return minScale_; }

	float GetDebufParm()const { return debufParm_; }

	void SetDebufParm(float parm) { debufParm_ = parm; }

	void SetPlayer(Player* _player);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// other class pointer
	Player* pPlayer_ = nullptr;

	/// tarnsform setting
	Quaternion quaternionLocalX_ = { 0.0f, 0.0f, 0.0f, 1.0f };
	Quaternion quaternionLocalY_ = { 0.0f, 0.0f, 0.0f, 1.0f };

	float localYAngle_ = 0.0f;

	/// action param setting
	float scaleScaler_ = 1.0f;
	float eacSpeed_; /// "eac == expansion and contraction"

	float maxScale_;
	float minScale_;

	float zRotateSpeed_ = 1.0f;

	std::vector<Wind*>             windArray_;
	std::vector<WindAnimationData> windAnimationDataArray_;
	std::vector<ParticleData>      particleDataArray_;
	std::vector<WindData>          windDataArray_;
	float debufParm_;


	Mat4 matRotate_;

};


/// ===================================================
/// 風
/// ===================================================
class Wind : public BaseGameObject {
public:

	Wind();
	~Wind() {}

	static void ResetInstanceCount();

	void Initialize() override;
	void Update()     override;

private:

	static int sInstanceCount_;
	int id_;

};