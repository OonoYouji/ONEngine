#pragma once

class BoxCollider;
class SphereCollider;
class CapsuleCollider;

namespace CollisionChecker {

	bool BoxToBox(BoxCollider* a, BoxCollider* b);
	bool SphereToSphere(SphereCollider* a, SphereCollider* b);
	bool CapsuleToCapsule(CapsuleCollider* a, CapsuleCollider* b);

	bool BoxToSphere(BoxCollider* box, SphereCollider* sphere);
	bool BoxToCapsule(BoxCollider* box, CapsuleCollider* capsule);
	bool SphereToCapsule(SphereCollider* sphere, CapsuleCollider* capsule);
	
}