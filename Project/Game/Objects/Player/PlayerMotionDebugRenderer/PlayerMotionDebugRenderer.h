#pragma once

#include "GameObjectManager/BaseGameObject.h"

class PlayerMotionDebugRenderer : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerMotionDebugRenderer(class Player* _player);
	~PlayerMotionDebugRenderer();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class SplinePathRenderer* splinePathRenderer_ = nullptr;
	class MeshInstancingRenderer* meshInstancingRenderer_ = nullptr;

	class Player* pPlayer_ = nullptr;

};
