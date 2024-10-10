#pragma once

#include "GameObjectManager/GameObjectManager.h"

/// ===================================================
/// 前方宣言
/// ===================================================
class Player;


/// ===================================================
/// 竜巻を表現するクラス
/// ===================================================
class Tornado final : public BaseGameObject {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	Tornado() { CreateTag(this); }
	~Tornado() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;


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

};