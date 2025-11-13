#pragma once

/// std
#include <deque>
#include <unordered_map>
#include <functional>
#include <string>

/// engine
#include "../Interface/ECSISystem.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Collision/ICollider.h"

/// ///////////////////////////////////////////////////
/// 衝突判定を計算したときの情報を格納する構造体
/// ///////////////////////////////////////////////////
struct CollisionInfo {
	Vector3 normal;       /// 衝突法線 (この方向に押し戻す)
	float penetration;    /// めり込み
	Vector3 contactPoint; /// 接触点
};

/// ///////////////////////////////////////////////////
/// 衝突判定の計算を行い、コールバック関数を呼び出すシステム
/// ///////////////////////////////////////////////////
class CollisionSystem : public ECSISystem {
public:
	/// =======================================
	/// public : methods
	/// =======================================

	CollisionSystem();
	virtual ~CollisionSystem() = default;

	void RuntimeUpdate(class ECSGroup* _ecs);

	/// コールバック関数の呼び出し
	void CallEnterFunc(const std::string& _ecsGroupName);
	void CallStayFunc(const std::string& _ecsGroupName);
	void CallExitFunc(const std::string& _ecsGroupName);

	/// @brief AとBの押し戻しを行う
	/// @param _a Aエンティティのポインタ
	/// @param _aState Aエンティティのコリジョン状態
	/// @param _b Bエンティティのポインタ
	/// @param _bState Bエンティティのコリジョン状態
	/// @param _info AとBの衝突情報
	void PushBack(
		class GameEntity* _a, CollisionState _aState,
		class GameEntity* _b, CollisionState _bState,
		const CollisionInfo& _info
	);
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
	using CollisionCheckFunc = std::function<bool(const CollisionPair&, CollisionInfo*)>;
	std::unordered_map<std::string, CollisionCheckFunc> collisionCheckMap_;

};


class SphereCollider;
class BoxCollider;

/*
* Check関数のA->Bに衝突しているかを判定する
* そのためCollisionInfoの法線情報がBからAへの法線になるように設定する
* B(Box) A(Sphere)のとき、法線は衝突した面の外向き法線になる
*/

namespace CheckMethod {
	bool CollisionCheckSphereVsSphere(SphereCollider* _s1, SphereCollider* _s2, CollisionInfo* _info);
	bool CollisionCheckSphereVsBox(SphereCollider* _s, BoxCollider* _b, CollisionInfo* _info);
	bool CollisionCheckBoxVsSphere(BoxCollider* _b, SphereCollider* _s, CollisionInfo* _info);
	bool CollisionCheckBoxVsBox(BoxCollider* _b1, BoxCollider* _b2, CollisionInfo* _info);
}