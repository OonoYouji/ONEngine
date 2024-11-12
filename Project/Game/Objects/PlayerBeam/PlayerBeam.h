#pragma once

/// std
#include <array>

#include "GameObjectManager/BaseGameObject.h"


enum BEAM_LEFT_AND_RIGHT {
	BEAM_LEFT,	/// 左から発射されている
	BEAM_RIGHT,	/// 右から発射されている
	BEAM_COUNT, /// 
};

class PlayerBeam : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerBeam();
	~PlayerBeam();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	void SetReticle(class Reticle* _reticle);

	void SetPlayer(class Player* _player);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class Reticle* pReticle_ = nullptr;
	class Player* pPlayer_ = nullptr;

	class MeshRenderer*    meshRenderer_    = nullptr;
	class CapsuleCollider* capsuleCollider_ = nullptr;

	Vec3 diff_;

	std::array<Vec3, 2> colliderPositionArray_;

};
