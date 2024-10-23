#pragma once

#include "GameObjectManager/GameObjectManager.h"

/// 前方宣言
class GameCamera;


class Reticle : public BaseGameObject {
public:

	Reticle();
	~Reticle();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	void SetGameCamera(GameCamera* _gameCamera);

private:

	GameCamera* pGameCamera_ = nullptr;

	Vec3  velocity_ = {};
	float speed_    = 1.0f;

};
