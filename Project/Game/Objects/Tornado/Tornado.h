#pragma once

/// std
#include <vector>

#include "GameObjectManager/GameObjectManager.h"

/// ===================================================
/// 前方宣言
/// ===================================================
class Player;
class Ring;
class Wind;

/// ===================================================
/// 竜巻を表現するクラス
/// ===================================================
class Tornado final : public BaseGameObject {

	struct ParticleData {
		float time = 0.0f;
		float speed = 1.0f;
		float radius = 1.0f;
		float maxPosY = 0.0f;
		Vec3 rotate{};
		Vec3 scale{};
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	Tornado() { CreateTag(this); }
	~Tornado() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	float GetScaleScaler()const { return scaleScaler_; }
	float GetMaxScale()const { return maxScale_; }
	float GetMinScale()const { return minScale_; }

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

	std::vector<Wind*>        windArray_;
	std::vector<ParticleData> particleDataArray_;

	Mat4 matRotate_;

};


/// ===================================================
/// わっか
/// ===================================================
class Ring : public BaseGameObject {
public:

	Ring();
	~Ring() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	static void ResetInstanceCount();

private:

	static int sInstanceCount_;
	int id_;

	float rotateSpeed_ = 0.0f;

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