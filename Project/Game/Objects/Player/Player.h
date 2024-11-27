#pragma once

#include "GameObjectManager/BaseGameObject.h"

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
	

	/// other class
	class GameCamera* pGameCamera_ = nullptr;

};
