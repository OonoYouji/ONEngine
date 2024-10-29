#pragma once


/// objects
#include "Enemy.h"

/// base class
#include "GameObjectManager/GameObjectManager.h"

class EnemyManager : public BaseGameObject {

	struct IOData {

	};

public:

	EnemyManager();
	~EnemyManager();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	void CreateEnemy(
		Enemy* _prefab,
		const Vec3& _scale,
		const Vec3& _rotate, 
		const Vec3& _position
	) const;

private:
	Enemy* prefabEnemy_ = nullptr;
};
