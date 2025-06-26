#pragma once

/// std
#include <deque>
#include <unordered_map>
#include <functional>
#include <string>

/// engine
#include "../Interface/ECSISystem.h"

class CollisionSystem : public ECSISystem {
public:
	/// =======================================
	/// public : methods
	/// =======================================

	CollisionSystem();
	virtual ~CollisionSystem() = default;

	// Update method to handle collision logic
	void Update(class EntityComponentSystem* _pEntityComponentSystem) override;

private:
	/// =======================================
	/// private : objects
	/// =======================================

	using CollisionPair = std::pair<class IEntity*, class IEntity*>;

	std::deque<CollisionPair> collisionPairs_; ///< Store pairs of colliding entities

	/// collision check 
	using CollisionCheckFunc = std::function<bool(const CollisionPair&)>;
	std::unordered_map<std::string, CollisionCheckFunc> collisionCheckMap_;

};


class SphereCollider;
class BoxCollider;

namespace CheckMethod {
	bool CollisionCheckSphereVsSphere(SphereCollider* _s1, SphereCollider* _s2);
	bool CollisionCheckSphereVsBox(SphereCollider* _s, BoxCollider* _b);
	bool CollisionCheckBoxVsBox(BoxCollider* _b1, BoxCollider* _b2);
}