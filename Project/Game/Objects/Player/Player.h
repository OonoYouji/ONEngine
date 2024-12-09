#pragma once

#include "GameObjectManager/BaseGameObject.h"

#include "Behavior/PlayerBehaviorManager.h"


struct CommonData {
	Vec3 direction; /// 向いているほう

	Vec3 velocity;	/// 移動方向
	float movementSpeed;
	Vec3 position;

	bool useGravity; /// 重力方向に落ちているか
};

class Player : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Player(class GameCamera* _gameCamera);
	~Player();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	void AddVariables();
	void ApplyVariables();

	void LoadingBehaviors();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unique_ptr<PlayerBehaviorManager> behaviorManager_;

	/// components
	class MeshRenderer* meshRenderer_ = nullptr;

	/// other class
	class GameCamera* pGameCamera_ = nullptr;

	/// parameters
	CommonData currentCommonData_;

public:

	CommonData& GetCommonData() { return currentCommonData_; }
	GameCamera* GetGameCamera() { return pGameCamera_; }

};
