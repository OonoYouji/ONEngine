#include "OBBCollider.h"

void OBBCollider::Initialize(BaseGameObject* gameObject, Model* model) {}

void OBBCollider::Update() {}

void OBBCollider::Draw() {}

bool OBBCollider::IsCollision(MAYBE_UNUSED BoxCollider* box) {
	return false;
}

bool OBBCollider::IsCollision(MAYBE_UNUSED AABBCollider* box) {
	return false;
}

bool OBBCollider::IsCollision(MAYBE_UNUSED OBBCollider* box) {
	return false;
}

bool OBBCollider::IsCollision(MAYBE_UNUSED SphereCollider* box) {
	return false;
}
