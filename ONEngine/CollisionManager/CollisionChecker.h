#pragma once

class BoxCollider;
class SphereCollider;


namespace CollisionChecker {

	bool BoxToBox(BoxCollider* a, BoxCollider* b);
	bool SphereToSphere(SphereCollider* a, SphereCollider* b);
	bool BoxToSphere(BoxCollider* box, SphereCollider* sphere);
}