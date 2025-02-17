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

	void ImGuiGizmo(
		Vec3* _position, Vec3* _rotate, Vec3* _scale,
		ROTATE_ORDER _rotateOrder
	);

private:

	class MeshInstancingRenderer* renderer_ = nullptr;
	float shininess;

	std::vector<Transform> transforms_;

	float maxFPS_, minFPS_;

};
