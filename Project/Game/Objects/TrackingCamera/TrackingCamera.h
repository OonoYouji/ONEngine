#pragma once

#include "GameObjectManager/BaseGameObject.h"

class TrackingCamera : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	TrackingCamera(class GameCamera* _gameCamera, class Player* _player, class Enemy* _enemy);
	~TrackingCamera();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;


	void ApplyVariables();
	void AddVariables();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================


	/// other class pointer
	class GameCamera* pGameCamera_ = nullptr; /// ゲームカメラへのポインタ
	class Player*     pPlayer_     = nullptr; /// プレイヤーへのポインタ
	class Enemy*      pEnemy_      = nullptr;

	Vec3 offsetPosition_; /// プレイヤーからどのくらい離れているか


};
