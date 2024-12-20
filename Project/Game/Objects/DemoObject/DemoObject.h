#pragma once

#include <imgui.h>
#include <ImGuizmo.h>

#include <vector>
#include <memory>

/// base class
#include "GameObjectManager/GameObjectManager.h"

#include "ComponentManager/AnimationRenderer/Skeleton.h"

class DemoObject final : public BaseGameObject {
public:

	DemoObject() { CreateTag(this); }
	~DemoObject() {}
	
	void Initialize() override;
	void Update()     override;
	void Debug()      override;

private:

	class MeshRenderer* renderer_ = nullptr;
	float shininess;

};
