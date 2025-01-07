#pragma once

#include "GameObjectManager/BaseGameObject.h"

class PlayerMoveLocus : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerMoveLocus(class Player* _player);
	~PlayerMoveLocus();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class MeshInstancingRenderer* renderer_ = nullptr;
	class Player* pPlayer_ = nullptr;

	std::array<Transform, 16> transforms_;

};
