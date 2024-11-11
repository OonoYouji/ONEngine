#pragma once

#include "GameObjectManager/BaseGameObject.h"

class SkyDome : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	SkyDome();
	~SkyDome();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	void SetOffsetObject(BaseGameObject* _offsetObject);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class MeshRenderer* meshRenderer_ = nullptr;

	BaseGameObject* pOffsetObject_ = nullptr;

	Vec2 uvScale_ = { 10.0f, 5.0f };
};
