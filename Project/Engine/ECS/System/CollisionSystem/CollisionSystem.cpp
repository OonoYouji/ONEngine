#include "CollisionSystem.h"

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

/// collider
#include "Engine/ECS/Component/Components/ComputeComponents/Collision/CollisionCheck/CollisionCheck.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Collision/SphereCollider.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Collision/BoxCollider.h"


CollisionSystem::CollisionSystem() {

	std::string sphereCompName = typeid(SphereCollider).name();
	std::string boxCompName = typeid(BoxCollider).name();

	/// 関数の登録をする
	collisionCheckMap_[sphereCompName + "Vs" + sphereCompName] = [](const CollisionPair& _pair) -> bool {
		return CheckMethod::CollisionCheckSphereVsSphere(
			_pair.first->GetComponent<SphereCollider>(),
			_pair.second->GetComponent<SphereCollider>()
		);
		};

	collisionCheckMap_[sphereCompName + "Vs" + boxCompName] = [](const CollisionPair& _pair) -> bool {
		return CheckMethod::CollisionCheckSphereVsBox(
			_pair.first->GetComponent<SphereCollider>(),
			_pair.second->GetComponent<BoxCollider>()
		);
		};
	
	collisionCheckMap_[boxCompName + "Vs" + sphereCompName] = [](const CollisionPair& _pair) -> bool {
		return CheckMethod::CollisionCheckSphereVsBox(
			_pair.first->GetComponent<SphereCollider>(),
			_pair.second->GetComponent<BoxCollider>()
		);
		};

	collisionCheckMap_[boxCompName + "Vs" + boxCompName] = [](const CollisionPair& _pair) -> bool {
		return CheckMethod::CollisionCheckBoxVsBox(
			_pair.first->GetComponent<BoxCollider>(),
			_pair.second->GetComponent<BoxCollider>()
		);
		};

}

void CollisionSystem::Update(EntityComponentSystem* _pEntityComponentSystem) {

	/// colliderを集める
	std::vector<ICollider*> colliders;
	for (auto& entity : _pEntityComponentSystem->GetEntities()) {

		/// sphere collider check
		SphereCollider* sphereCollider = entity->GetComponent<SphereCollider>();
		if (sphereCollider) {
			colliders.push_back(sphereCollider);
			continue;
		}

		/// box collider check
		BoxCollider* boxCollider = entity->GetComponent<BoxCollider>();
		if (boxCollider) {
			colliders.push_back(boxCollider);
			continue;
		}
	}


	std::unordered_map<std::string, int> collisionFrameMap; ///< 衝突計算をしたフレームを記録するマップ

	/// 衝突判定
	for (auto& a : colliders) {
		for (auto& b : colliders) {

			/// 同じオブジェクト同士の衝突は無視
			if (a == b) {
				continue;
			}

			/// このフレームないで衝突計算をしているかチェック
			std::string str = typeid(*a).name() + std::string("Vs") + typeid(*b).name();
			if (collisionFrameMap[str] == 0) {
				++collisionFrameMap[str]; // 衝突計算をしたフレームを記録

				/// 衝突計算を行う
				CollisionPair pair(a->GetOwner(), b->GetOwner());
				auto it = collisionCheckMap_.find(str);
				if (it != collisionCheckMap_.end()) {
					bool isCollided = it->second(pair);
					if (isCollided) {
						/// 衝突している場合はペアを記録
						collisionPairs_.emplace_back(pair);
					}
				} else {
					// 衝突計算が登録されていない場合の処理（必要に応じて）
				}
			}

		}
	}


}


bool CheckMethod::CollisionCheckSphereVsSphere(SphereCollider* _s1, SphereCollider* _s2) {
	if (!_s1 || !_s2) {
		return false; // 型が一致しない場合は衝突なし
	}

	IEntity* e1 = _s1->GetOwner();
	IEntity* e2 = _s2->GetOwner();

	float distance = (e1->GetPosition() - e2->GetPosition()).Len();
	return distance <= (_s1->GetRadius() + _s2->GetRadius());
}

bool CheckMethod::CollisionCheckSphereVsBox(SphereCollider* _s, BoxCollider* _b) {
	if (!_s || !_b) {
		return false; // 型が一致しない場合は衝突なし
	}
	IEntity* e1 = _s->GetOwner();
	IEntity* e2 = _b->GetOwner();
	return CollisionCheck::CubeVsSphere(
		e2->GetPosition(), _b->GetSize(),
		e1->GetPosition(), _s->GetRadius()
	);
}

bool CheckMethod::CollisionCheckBoxVsBox(BoxCollider* _b1, BoxCollider* _b2) {
	if (!_b1 || !_b2) {
		return false; // 型が一致しない場合は衝突なし
	}
	IEntity* e1 = _b1->GetOwner();
	IEntity* e2 = _b2->GetOwner();
	return CollisionCheck::CubeVsCube(
		e1->GetPosition(), _b1->GetSize(),
		e2->GetPosition(), _b2->GetSize()
	);
}

