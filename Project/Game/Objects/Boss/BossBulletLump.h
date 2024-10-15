#pragma once

/// std
#include <vector>

#include "GameObjectManager/GameObjectManager.h"

/// ===================================================
/// 前方宣言
/// ===================================================
class Boss;
/// ===================================================
/// 竜巻を表現するクラス
/// ===================================================
class BossBulletLump final : public BaseGameObject {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	BossBulletLump() { CreateTag(this); }
	~BossBulletLump() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	//setter^d
	void SetDirection(Quaternion direction);
	void SetPivotQuaternion(Quaternion pivot) { pivot_.quaternion = pivot; }
	void SetQuaternionSubtraction(Quaternion pivot) { pivot_.quaternion *= pivot; }

	void SetBoss(Boss* boss);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// other class pointer
	Boss* pBoss_ = nullptr;

	/// tarnsform setting
	Quaternion quaternionLocalX_ = { 0.0f, 0.0f, 0.0f, 1.0f };
	Quaternion quaternionLocalY_ = { 0.0f, 0.0f, 0.0f, 1.0f };

	Transform pivot_;
	
	float localYAngle_ = 0.0f;

	/// action param setting
	float scaleScaler_ = 1.0f;
	float eacSpeed_; /// "eac == expansion and contraction"

	float maxScale_;
	float minScale_;

	float zRotateSpeed_ = 1.0f;
};
