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
namespace ONEngine {

struct CollisionInfo {
	Vector3 normal;       /// 衝突法線 (この方向に押し戻す)
	float penetration;    /// めり込み
	Vector3 contactPoint; /// 接触点
};

/// ///////////////////////////////////////////////////
/// 衝突判定の計算を行い、コールバック関数を呼び出すシステム
/// ///////////////////////////////////////////////////
/**
 * @class CollisionSystem
 * @brief エンティティ間のコライダー衝突判定、押し戻し（プッシュバック）処理、および衝突判定に基づくコールバック（OnCollisionEnter/Stay/Exit）を毎フレーム制御するシステムクラス
 */
class CollisionSystem : public ECSISystem {
public:
	/// =======================================
	/// public : methods
	/// =======================================

	/**
	 * @brief コンストラクタ
	 */
	CollisionSystem();

	/**
	 * @brief デストラクタ
	 */
	virtual ~CollisionSystem() = default;

	/**
	 * @brief 毎フレームのコライダー衝突判定と押し戻し、衝突ステート遷移計算を実行します。
	 * @param _ecs 対象となるECSグループへのポインタ
	 */
	void RuntimeUpdate(class ECSGroup* _ecs);

	/**
	 * @brief 新たに衝突を開始した（Enter状態の）エンティティペアのC#イベントを通知します。
	 */
	void CallEnterFunc(const std::string& _ecsGroupName);

	/**
	 * @brief 衝突を継続している（Stay状態の）エンティティペアのC#イベントを通知します。
	 */
	void CallStayFunc(const std::string& _ecsGroupName);

	/**
	 * @brief 衝突を終了した（Exit状態の）エンティティペアのC#イベントを通知します。
	 */
	void CallExitFunc(const std::string& _ecsGroupName);

	/**
	 * @brief 衝突したエンティティ同士の押し戻し（プッシュバック）処理を行います。
	 * @param _a 衝突側エンティティ A へのポインタ
	 * @param _aState エンティティ A のコリジョン状態（静的/動的）
	 * @param _b 被衝突側エンティティ B へのポインタ
	 * @param _bState エンティティ B のコリジョン状態
	 * @param _info 衝突時の接触面法線や貫通深度を含む衝突情報
	 */
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
    /**
     * @brief 球体と球体の衝突判定を行います。
     */
	bool CollisionCheckSphereVsSphere(SphereCollider* _s1, SphereCollider* _s2, CollisionInfo* _info);

    /**
     * @brief 球体とボックスの衝突判定を行います。
     */
	bool CollisionCheckSphereVsBox(SphereCollider* _s, BoxCollider* _b, CollisionInfo* _info);

    /**
     * @brief ボックスと球体の衝突判定を行います。
     */
	bool CollisionCheckBoxVsSphere(BoxCollider* _b, SphereCollider* _s, CollisionInfo* _info);

    /**
     * @brief ボックスとボックスの衝突判定を行います。
     */
	bool CollisionCheckBoxVsBox(BoxCollider* _b1, BoxCollider* _b2, CollisionInfo* _info);
}

} /// ONEngine
