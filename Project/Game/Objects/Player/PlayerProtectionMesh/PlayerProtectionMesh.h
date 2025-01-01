#pragma once

#include "GameObjectManager/BaseGameObject.h"

class PlayerProtectionMesh : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerProtectionMesh(class Player* _playerPtr);
	~PlayerProtectionMesh();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	void AddVariables();
	void ApplyVariables();
	void LoadVariables();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class MeshRenderer* meshRenderer_ = nullptr;
	class Player*       pPlayer_      = nullptr;

	class BoxCollider*  boxCollider_  = nullptr;

};
