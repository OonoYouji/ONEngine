#pragma once

#include <list>
#include <memory>
#include <deque>

#include <BaseGameObject.h>

class BaseGameObject;

class CollisionManager final {
	CollisionManager() = default;
	~CollisionManager() = default;
public:

	static CollisionManager* GetInstance() {
		static CollisionManager instance;
		return &instance;
	}


	void AddGameObject(BaseGameObject* collider);
	void SubGameObject(BaseGameObject* collider);

	void Update(int currentSceneId);

	void DrawHitBoxALL();
	
	void CheckCollision(BaseGameObject* a, BaseGameObject* b);

	void ImGuiDebug();

	void Reset();

private:

	using CollidedPair = std::pair<BaseGameObject*, BaseGameObject*>;

	std::list<BaseGameObject*> gameObjects_;

	std::list<CollidedPair> collidedPairs_;
	std::list<CollidedPair> currentCollidedPairs_; /// 1frame間で衝突したもののみpush_back()

#ifdef _DEBUG
	std::deque<std::string> pairNames_;
#endif // _DEBUG

};