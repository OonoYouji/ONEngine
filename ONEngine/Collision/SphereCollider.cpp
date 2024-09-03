#include "SphereCollider.h"


void SphereCollider::Initialize(BaseGameObject* gameObject, Model* model) {

}

void SphereCollider::Update() {

}

void SphereCollider::Draw() {

}


bool SphereCollider::IsCollision(MAYBE_UNUSED BoxCollider* box) {
	return false;
}

bool SphereCollider::IsCollision(MAYBE_UNUSED AABBCollider* box) {
	return false;
}

bool SphereCollider::IsCollision(MAYBE_UNUSED OBBCollider* box) {
	return false;
}

bool SphereCollider::IsCollision(MAYBE_UNUSED SphereCollider* box) {
	return false;
}
