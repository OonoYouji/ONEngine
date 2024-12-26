#pragma once

/// engine
#include "GameObjectManager/BaseGameObject.h"

/// user
#include "PlayerMesh/PlayerMesh.h"


/// ===================================================
/// プレイヤーのクラス
/// ===================================================
class Player : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Player(class GameCamera* _gameCameraPtr);
	~Player();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	PlayerMesh* playerMesh_ = nullptr;

};
