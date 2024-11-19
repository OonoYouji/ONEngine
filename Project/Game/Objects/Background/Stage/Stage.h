#pragma once

#include "GameObjectManager/BaseGameObject.h"

class Stage : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Stage();
	~Stage();

	void Initialize() override;
	void Update()     override;


	void AddVariables();
	void ApplyVariables();


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class MeshRenderer* meshRenderer_ = nullptr;

	Vec2 uvScale_;

};
