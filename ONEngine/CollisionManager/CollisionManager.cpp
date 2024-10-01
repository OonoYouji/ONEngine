#include "CollisionManager.h"

#include <ImGuiManager.h>

#include <CollisionManager/CollisionChecker.h>
#include <Component/Collider/BoxCollider.h>


CollisionManager CollisionManager::instance_;



void CollisionManager::Initialize() {

}

void CollisionManager::Finalize() {

}

void CollisionManager::Update() {

	for(auto& objA : gameObjects_) {
		for(auto& objB : gameObjects_) {

			/// 同じオブジェクトなら除外
			if(objA == objB) {
				continue;
			}

			/// このフレーム内ですでに当たっているかチェック
			auto itr = std::find_if(currentCollisionPair_.begin(), currentCollisionPair_.end(), [&](const CollisionPair& pair) {
				return (pair.first == objA && pair.second == objB)
					|| (pair.first == objB && pair.second == objA);
			});

			/// 当たっているので除外
			if(itr != currentCollisionPair_.end()) {
				continue;
			}

			/// 当たり判定チェック
			CheckCollision(objA, objB);

		}
	}

}

void CollisionManager::ImGuiDebug() {
	if(!ImGui::Begin("CollisionManager")) {
		ImGui::End();
		return;
	}

	/// 

	ImGui::End();
}



void CollisionManager::CheckCollision(BaseGameObject* objA, BaseGameObject* objB) {
	/// objA, objBの当たり判定をする

	BaseCollider* colliderA = objA->GetComponent<BaseCollider>();
	BaseCollider* colliderB = objB->GetComponent<BaseCollider>();

	/// 当たったら
	//collisionPair_.push_back(std::make_pair(objA, objB));

}

