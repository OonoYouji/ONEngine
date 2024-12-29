#pragma once

#include "GameObjectManager/BaseGameObject.h"

class EntityShadow : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	EntityShadow(BaseGameObject* _entity, Model* _model);
	~EntityShadow();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	BaseGameObject* pEntity_ = nullptr;
	Model*          pModel_  = nullptr;

	class MeshRenderer* meshRenderer_ = nullptr;

};
