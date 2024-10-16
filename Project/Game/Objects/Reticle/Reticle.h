#pragma once

#include "GameObjectManager/GameObjectManager.h"

/// 前方宣言
class GameCamera;


class Reticle : BaseGameObject {
public:

	Reticle();
	~Reticle();

	void Initialize() override;
	void Update()     override;


	void SetGameCamera(GameCamera* _gameCamera);

private:

	GameCamera* pGameCamera_ = nullptr;

};
