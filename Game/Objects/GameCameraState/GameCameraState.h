#pragma once

#include "GameObjectManager.h"

#include "Camera/GameCamera.h"


/// ===================================================
/// 前方宣言
/// ===================================================

class Player;



/// ===================================================
/// ゲームカメラのアクションを行うクラス
/// ===================================================
class GameCameraState final : public BaseGameObject {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	GameCameraState() { CreateTag(this); }
	~GameCameraState() {}

	void Initialize() override;
	void Update()     override;
	void LastUpdate() override;
	void Debug()      override;

	void SetGameCamera(GameCamera* _gameCamera);
	void SetPlayer(Player* _player);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	Player*     pPlayer_     = nullptr;
	GameCamera* pGameCamera_ = nullptr;

	/// player...
	Vec3 playerForward_{};

	/// camera...
	Vec3  cameraUp_{};
	float cameraRotateZ_ = 0.0f;

};