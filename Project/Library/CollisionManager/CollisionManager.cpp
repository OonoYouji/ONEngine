#include "CollisionManager.h"

#include <algorithm>

#include "ImGuiManager/ImGuiManager.h"

#include "CollisionManager/CollisionChecker.h"
#include "ComponentManager/Collider/BoxCollider.h"
#include "ComponentManager/Collider/SphereCollider.h"


CollisionManager CollisionManager::instance_;



void CollisionManager::Initialize() {

}

void CollisionManager::Finalize() {

}

void CollisionManager::Update() {

	currentCollisionPair_.clear();

	for(auto& objA : gameObjects_) {

		/// colliderがあるかチェック
		BaseCollider* colliderA = objA->GetComponent<BaseCollider>();
		if(!colliderA) {
			continue;
		}

		for(auto& objB : gameObjects_) {

			/// colliderがあるかチェック
			BaseCollider* colliderB = objB->GetComponent<BaseCollider>();
			if(!colliderB) {
				continue;
			}


			/// 同じオブジェクトなら除外
			if(objA == objB) {
				continue;
			}

			/// 同じtagなら除外
			if(objA->GetTag() == objB->GetTag()) {
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
			CheckCollision(
				objA, colliderA,
				objB, colliderB
			);

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

void CollisionManager::AddGameObject(BaseGameObject* obj) {
	gameObjects_.push_back(obj);
}

void CollisionManager::SubGameObject(BaseGameObject* obj) {
	auto itr = std::find(gameObjects_.begin(), gameObjects_.end(), obj);
	gameObjects_.erase(itr);
}


void CollisionManager::Reset() {
	gameObjects_.clear();
	currentCollisionPair_.clear();
	collisionPair_.clear();
}



void CollisionManager::CheckCollision(BaseGameObject* objA, BaseCollider* colliderA, BaseGameObject* objB, BaseCollider* colliderB) {

	/// objA, objBの当たり判定をする
	bool isCollision = false;

	/// colliderAがBoxColliderのとき
	if(BoxCollider* boxA = dynamic_cast<BoxCollider*>(colliderA)) {
		if(BoxCollider* boxB = dynamic_cast<BoxCollider*>(colliderB)) {
			isCollision = CollisionChecker::BoxToBox(boxA, boxB);

		} else if(SphereCollider* sphereB = dynamic_cast<SphereCollider*>(colliderB)) {
			isCollision = CollisionChecker::BoxToSphere(boxA, sphereB);
		}
	}


	/// 上で当たってなければ判定を取る
	if(!isCollision) {
		if(SphereCollider* sphereA = dynamic_cast<SphereCollider*>(colliderA)) {
			if(BoxCollider* boxB = dynamic_cast<BoxCollider*>(colliderB)) {
				isCollision = CollisionChecker::BoxToSphere(boxB, sphereA);

			} else if(SphereCollider* sphereB = dynamic_cast<SphereCollider*>(colliderB)) {

				isCollision = CollisionChecker::SphereToSphere(sphereA, sphereB);
			}
		}


		/// ここまできて当たっていなければ終了!!!
		if(!isCollision) {
			ErasePair(objA, objB);
			return;
		}
	}


	collisionPair_.push_back(std::make_pair(objA, objB));
	currentCollisionPair_.push_back(std::make_pair(objA, objB));
	CheckCallbackMethod(objA, objB);


}

void CollisionManager::CheckCallbackMethod(BaseGameObject* objA, BaseGameObject* objB) {

	uint32_t instanceCount = static_cast<uint32_t>(std::count_if(collisionPair_.begin(), collisionPair_.end(), [&](const CollisionPair& pair) {
		return (pair.first == objA && pair.second == objB)
			|| (pair.first == objB && pair.second == objA);
	}));


	if(instanceCount == 0) {
		objA->OnCollisionExit(objB);
		objB->OnCollisionExit(objA);
	} else if(instanceCount == 1) {
		objA->OnCollisionEnter(objB);
		objB->OnCollisionEnter(objA);
	} else {
		objA->OnCollisionStay(objB);
		objB->OnCollisionStay(objA);
	}


}

void CollisionManager::ErasePair(BaseGameObject* objA, BaseGameObject* objB) {

	bool hasPair = false;

	collisionPair_.remove_if([&](const CollisionPair& pair) {
		if((pair.first == objA && pair.second == objB)
		   || (pair.first == objB && pair.second == objA)) {
			hasPair = true;
			return true;
		}
		return false;
	});
	
	currentCollisionPair_.remove_if([&](const CollisionPair& pair) {
		return (pair.first == objA && pair.second == objB)
			|| (pair.first == objB && pair.second == objA);
	});



	if(hasPair) {
		CheckCallbackMethod(objA, objB);
	}

}

