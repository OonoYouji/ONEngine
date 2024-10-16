#pragma once

#include <vector>
#include <memory>

#include "GameObjectManager/GameObjectManager.h"


class DemoObject final : public BaseGameObject {
public:

	DemoObject() { CreateTag(this); }
	~DemoObject() {}
	
	void Initialize() override;
	void Update()     override;
	void Debug()      override;

private:

	std::vector<std::unique_ptr<Transform>> transformArray_;

};