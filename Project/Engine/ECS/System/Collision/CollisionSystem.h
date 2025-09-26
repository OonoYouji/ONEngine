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

	void OutsideOfRuntimeUpdate(class ECSGroup* _ecs) override;
	void RuntimeUpdate(class ECSGroup* _ecs);

	void CallEnterFunc(const std::string& _ecsGroupName);
	void CallStayFunc(const std::string& _ecsGroupName);
	void CallExitFunc(const std::string& _ecsGroupName);

private:
	/// =======================================
	/// private : objects
	/// =======================================

	using CollisionPair = std::pair<class GameEntity*, class GameEntity*>;

	std::deque<CollisionPair> collidedPairs_;

	/// ----- call back ----- ///
	std::deque<CollisionPair> enterPairs_; /// 衝突が開始したペア
	std::deque<CollisionPair> stayPairs_;  /// 衝突が継続しているペア
	std::deque<CollisionPair> exitPairs_;  /// 衝突が終了したペア


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