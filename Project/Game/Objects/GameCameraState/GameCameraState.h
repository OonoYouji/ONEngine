#pragma once

/// objects
#include "GameObjectManager/BaseGameObject.h"
#include "Objects/Camera/GameCamera.h"


/// ===================================================
/// 前方宣言
/// ===================================================

class Player;
class DirectionalLight;


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
	void SetDirectionalLight(DirectionalLight* _directionalLight);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// other class pointer
	Player*           pPlayer_     = nullptr;
	GameCamera*       pGameCamera_ = nullptr;
	DirectionalLight* pLight_      = nullptr;

};