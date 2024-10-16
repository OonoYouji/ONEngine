#pragma once


/// component
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

/// base class
#include "GameObjectManager/GameObjectManager.h"


class BossParts : public BaseGameObject {
public:

	BossParts();
	~BossParts();

	void Initialize() override;
	void Update()     override;

	
	void SetModelName(const std::string& _modelName);

private:

	MeshRenderer* meshRenderer_ = nullptr;
	std::string   modelName_;

};
