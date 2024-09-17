#pragma once

#include <list>

#include <GameObjectManager.h>


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
	std::list<CollisionPair> pair_;

	Vec3 division_;

};