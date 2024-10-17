#pragma once

#include "GameObjectManager/BaseGameObject.h"

/// 前方宣言
class GameCamera;

class CameraStateBossEntryToGame final : public BaseGameObject {

	struct MovementData {
		Vec3 position;
		Vec3 rotate;
	};

public:

	CameraStateBossEntryToGame() { CreateTag(this); }
	~CameraStateBossEntryToGame() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	void SetGameCamera(GameCamera* _gameCamera);

	bool IsFinishedMoving();

private:
	
	GameCamera* pGameCamera_ = nullptr;

	MovementData startData_;
	MovementData endData_;

	float moveTime_;
	float maxMoveTime_;

};