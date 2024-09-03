#include "BaseCollider.h"

#include <GameObjectManager.h>
#include <Model.h>

#include "BoxCollider.h"
#include "SphereCollider.h"
#include "AABBCollider.h"
#include "OBBCollider.h"


void BaseCollider::Initialize(BaseGameObject* gameObject, [[maybe_unused]] Model* model) {
	gameObject_ = gameObject;
}

bool BaseCollider::IsCollision(MAYBE_UNUSED BaseCollider* other) {
	
	BoxCollider* box = dynamic_cast<BoxCollider*>(other);
	if(box) {
		return IsCollision(box);
	}

	SphereCollider* sphere = dynamic_cast<SphereCollider*>(other);
	if(sphere) {
		return IsCollision(sphere);
	}

	AABBCollider* aabb = dynamic_cast<AABBCollider*>(other);
	if(aabb) {
		return IsCollision(aabb);
	}

	OBBCollider* obb = dynamic_cast<OBBCollider*>(other);
	if(obb) {
		return IsCollision(obb);
	}


	return false;
}

void BaseCollider::UpdateMatrix() {
	transform_.UpdateMatrix();
	transform_.matTransform *= gameObject_->GetMatTransform();
}
