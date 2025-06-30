#include "SphereCollider.h"

/// engine
#include "Engine/Core/Utility/Utility.h"

SphereCollider::SphereCollider() {
	// デフォルトの値をセット
	radius_ = 1.0f; 
}

void SphereCollider::SetRadius(float _radius) {
	radius_ = _radius;
}

float SphereCollider::GetRadius() const {
	return radius_;
}

void COMP_DEBUG::SphereColliderDebug(SphereCollider* _collider) {
	if (!_collider) {
		return;
	}


	float radius = _collider->GetRadius();
	if (ImGui::DragFloat("Radius", &radius, 0.1f, 0.0f, 100.0f)) {
		_collider->SetRadius(radius);
	}
	
}
