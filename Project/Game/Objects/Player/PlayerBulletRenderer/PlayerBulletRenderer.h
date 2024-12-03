#pragma once

/// std
#include <list>

/// engine
#include "GameObjectManager/BaseGameObject.h"

class PlayerBulletRenderer : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerBulletRenderer();
	~PlayerBulletRenderer();

	void Initialize() override;
	void Update()     override;

	void PushBackBullet(class PlayerBullet* _bullet);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class MeshInstancingRenderer* instancingRenderer_ = nullptr;

	std::list<class PlayerBullet*> bullets_;

};
