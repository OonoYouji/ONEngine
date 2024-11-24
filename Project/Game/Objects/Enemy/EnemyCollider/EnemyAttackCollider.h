#pragma once

#include <Array>

#include "GameObjectManager/BaseGameObject.h"

#include "../BehaviorWorker/EnemyBehaviorWorkers.h"

enum class ActionTypes : int32_t;

class Enemy;
class EnemyAttackCollider
	: public BaseGameObject{
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	EnemyAttackCollider();
	~EnemyAttackCollider();

	void Initialize() override;
	void Update()     override;

	void OnCollisionEnter(BaseGameObject* const _collision) override;

	void ApplyVariables();
private:
	/// ===================================================
	/// private : objects
	/// ===================================================
	Enemy* enemy_ = nullptr;
	class SphereCollider* sphereCollider_ = nullptr;

	/// <summary>
	/// 攻撃の種類ごとに offsetを決める
	/// </summary>
	std::array<Vector3,static_cast<int32_t>(ActionTypes::COUNT)> offsetByActionTypes_;
public:
	void SetEnemy(Enemy* enemy);
	void Activate(ActionTypes actionType,float radius);
	void Terminate();
};
