#pragma once

#include <vector>
#include <memory>

/// component
#include "ComponentManager/ParticleSystem/ParticleSystem.h"

/// base class
#include "GameObjectManager/GameObjectManager.h"


class DemoObject final : public BaseGameObject {
public:

	DemoObject() { CreateTag(this); }
	~DemoObject() {}
	
	void Initialize() override;
	void Update()     override;
	void Debug()      override;

private:

	std::unique_ptr<ParticleField> particleField_;

};