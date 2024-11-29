#pragma once

#include "GameObjectManager/BaseGameObject.h"

/// base
#include "GameObjectManager/GameObjectManager.h"

/// ===================================================
/// 繝励Ξ繧､繝､繝ｼ縺梧桃菴懊☆繧九お繝ｳ繝・ぅ繝・ぅ縺ｮ繧ｯ繝ｩ繧ｹ
/// ===================================================
class Player : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Player(class GameCamera* _gameCamera);
	~Player();

	void Initialize() override;
	void Update()     override;

	void AddVariables();
	void ApplyVariables();

	void Movement();
	void Rotation();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class MeshRenderer* meshRenderer_ = nullptr;


	float movementSpeed_;
	Vec3  velocity_;
	Vec3  prevDirection_;
	float rotateSpeed_;

	/// bullet param
	float bulletSpeed_;

	/// other class
	class GameCamera* pGameCamera_ = nullptr;

};