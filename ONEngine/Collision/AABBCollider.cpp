#include "AABBCollider.h"


void AABBCollider::Initialize(BaseGameObject* gameObject, Model* model) {}

void AABBCollider::Update() {}

void AABBCollider::Draw() {}



bool AABBCollider::IsCollision(MAYBE_UNUSED BoxCollider* box) {
	return false;
}

bool AABBCollider::IsCollision(MAYBE_UNUSED AABBCollider* box) {
	return false;
}

bool AABBCollider::IsCollision(MAYBE_UNUSED OBBCollider* box) {
	return false;
}

bool AABBCollider::IsCollision(MAYBE_UNUSED SphereCollider* box) {
	return false;
}
