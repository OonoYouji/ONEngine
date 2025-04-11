#include "ImGuiMath.h"

/// std
#include <numbers>

/// engine
#include "Engine/ECS/Component/Components/ComputeComponents/Transform/Transform.h"


namespace {

	float rotateSpeed = std::numbers::pi_v<float> / 100.0f;

}	/// unnamed namespace


void TransformDebug(Transform* _transform) {

	ImGui::DragFloat3("position", &_transform->position.x, 0.1f);
	ImGui::DragFloat3("rotate",   &_transform->rotate.x,   rotateSpeed);
	ImGui::DragFloat3("scale",    &_transform->scale.x,    0.1f);
}
