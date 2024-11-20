#include "CollisionManager.h"

#include <algorithm>

#include "ImGuiManager/ImGuiManager.h"

#include "CollisionManager/CollisionChecker.h"
#include "ComponentManager/Collider/BoxCollider.h"
#include "ComponentManager/Collider/SphereCollider.h"
#include "ComponentManager/Collider/CapsuleCollider.h"


CollisionManager CollisionManager::instance_;



void CollisionManager::Initialize() {

	/// box to ???
	collisionCheckFuncs_[COLLIDER_TYPE_BOX][COLLIDER_TYPE_BOX] = [](BaseCollider* a, BaseCollider* b) {
		return CollisionChecker::BoxToBox(static_cast<BoxCollider*>(a), static_cast<BoxCollider*>(b));
	};
	collisionCheckFuncs_[COLLIDER_TYPE_BOX][COLLIDER_TYPE_SPHERE] = [](BaseCollider* a, BaseCollider* b) {
		return CollisionChecker::BoxToSphere(static_cast<BoxCollider*>(a), static_cast<SphereCollider*>(b));
	};
	collisionCheckFuncs_[COLLIDER_TYPE_BOX][COLLIDER_TYPE_CAPSULE] = [](BaseCollider* a, BaseCollider* b) {
		return CollisionChecker::BoxToCapsule(static_cast<BoxCollider*>(a), static_cast<CapsuleCollider*>(b));
	};

	/// sphere to ???
	collisionCheckFuncs_[COLLIDER_TYPE_SPHERE][COLLIDER_TYPE_SPHERE] = [](BaseCollider* a, BaseCollider* b) {
		return CollisionChecker::SphereToSphere(static_cast<SphereCollider*>(a), static_cast<SphereCollider*>(b));
	};
	collisionCheckFuncs_[COLLIDER_TYPE_SPHERE][COLLIDER_TYPE_BOX] = [](BaseCollider* a, BaseCollider* b) {
		return CollisionChecker::BoxToSphere(static_cast<BoxCollider*>(b), static_cast<SphereCollider*>(a));
	};
	collisionCheckFuncs_[COLLIDER_TYPE_SPHERE][COLLIDER_TYPE_CAPSULE] = [](BaseCollider* a, BaseCollider* b) {
		return CollisionChecker::SphereToCapsule(static_cast<SphereCollider*>(a), static_cast<CapsuleCollider*>(b));
	};

	/// capsule to ???
	collisionCheckFuncs_[COLLIDER_TYPE_CAPSULE][COLLIDER_TYPE_CAPSULE] = [](BaseCollider* a, BaseCollider* b) {
		return CollisionChecker::CapsuleToCapsule(static_cast<CapsuleCollider*>(a), static_cast<CapsuleCollider*>(b));
	};
	collisionCheckFuncs_[COLLIDER_TYPE_CAPSULE][COLLIDER_TYPE_BOX] = [](BaseCollider* a, BaseCollider* b) {
		return CollisionChecker::BoxToCapsule(static_cast<BoxCollider*>(b), static_cast<CapsuleCollider*>(a));
	};
	collisionCheckFuncs_[COLLIDER_TYPE_CAPSULE][COLLIDER_TYPE_SPHERE] = [](BaseCollider* a, BaseCollider* b) {
		return CollisionChecker::SphereToCapsule(static_cast<SphereCollider*>(b), static_cast<CapsuleCollider*>(a));
	};

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

		/// オブジェクト本体かコライダーのアクティブがfalseならcontinue
		if(!objA->isActive || !colliderA->isActive) {
			continue;
		}


		for(auto& objB : gameObjects_) {

			/// 同じオブジェクトなら除外
			if(objA == objB) {
				continue;
			}

			/// colliderがあるかチェック
			BaseCollider* colliderB = objB->GetComponent<BaseCollider>();
			if(!colliderB) {
				continue;
			}

			/// オブジェクト本体かコライダーのアクティブがfalseならcontinue
			if(!objB->isActive || !colliderB->isActive) {
				continue;
			}




			/// 違うレイヤーなら除外
			if(objA->drawLayerId != objB->drawLayerId) {
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

	int aType = colliderA->GetColliderType();
	int bType = colliderB->GetColliderType();

	/// objA, objBの当たり判定をする
	bool isCollision = collisionCheckFuncs_[aType][bType](
		colliderA, colliderB
	);

	/// 当たっていなければ終了
	if(!isCollision) {
		ErasePair(objA, objB);
		return;
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

