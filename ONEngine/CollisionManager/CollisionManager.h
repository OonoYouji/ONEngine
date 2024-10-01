#pragma once

#include <list>

#include <GameObjectManager.h>
#include <Component/Collider/BaseCollider.h>


/// ===================================================
/// 当たり判定の管理クラス
/// ===================================================
class CollisionManager final {
	CollisionManager() = default;
	~CollisionManager() = default;
public:
	/// ===================================================
	/// public : static methods
	/// ===================================================

	static CollisionManager* GetInstance() {
		return &instance_;
	}


	/// ===================================================
	/// public : methods
	/// ===================================================

	void Initialize();

	void Finalize();

	void Update();

	void ImGuiDebug();

	void AddGameObject(BaseGameObject* obj);
	void SubGameObject(BaseGameObject* obj);

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	void CheckCollision(
		BaseGameObject* objA, BaseCollider* colliderA,
		BaseGameObject* objB, BaseCollider* colliderB
	);

	/// <summary>
	/// どのコールバック関数を呼び出すか確認
	/// </summary>
	/// <param name="objA"></param>
	/// <param name="objB"></param>
	void CheckCallbackMethod(BaseGameObject* objA, BaseGameObject* objB);

	/// <summary>
	/// 当たったリストからこのpairを削除する
	/// </summary>
	/// <param name="objA"></param>
	/// <param name="objB"></param>
	void ErasePair(BaseGameObject* objA, BaseGameObject* objB);

private:
	/// ===================================================
	/// private : static objects
	/// ===================================================

	static CollisionManager instance_;


	/// ===================================================
	/// private : objects
	/// ===================================================

	std::list<BaseGameObject*> gameObjects_;

	using CollisionPair = std::pair<BaseGameObject*, BaseGameObject*>;
	std::list<CollisionPair> collisionPair_;

	/// このフレーム内で当たったオブジェクト
	std::list<CollisionPair> currentCollisionPair_;


};