#include "CollisionSystem.h"

/// std
#include <unordered_map>
#include <utility>

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Component.h"


namespace std {
	template <>
	struct hash<std::pair<int, int>> {
		std::size_t operator()(const std::pair<int, int>& p) const {
			return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
		}
	};
}


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

void CollisionSystem::RuntimeUpdate(ECSGroup* _ecs) {

	ComponentArray<SphereCollider>* sphereColliderArray = _ecs->GetComponentArray<SphereCollider>();
	ComponentArray<BoxCollider>*    boxColliderArray    = _ecs->GetComponentArray<BoxCollider>();

	/// コライダーの配列
	std::vector<ICollider*> colliders; 

	/// sphere colliderを配列に格納する、インスタンスのnullチェックと有効フラグのチェックを行う
	if (sphereColliderArray) {
		for (auto& sphereCollider : sphereColliderArray->GetUsedComponents()) {
			if (sphereCollider && sphereCollider->enable) {
				colliders.push_back(sphereCollider);
			}
		}
	}

	/// box colliderを配列に格納する、インスタンスのnullチェックと有効フラグのチェックを行う
	if (boxColliderArray) {
		for (auto& boxCollider : boxColliderArray->GetUsedComponents()) {
			if (boxCollider && boxCollider->enable) {
				colliders.push_back(boxCollider);
			}
		}
	}


	using EntityPair = std::pair<int, int>;
	std::unordered_map<EntityPair, int> collisionFrameMap; ///< 衝突計算をしたフレームを記録するマップ

	/// 衝突判定
	for (auto& a : colliders) {
		for (auto& b : colliders) {

			/// 同じオブジェクト同士の衝突は無視
			if (a == b) {
				continue;
			}

			/// このフレームないで衝突計算をしているかチェック
			std::string collisionType = typeid(*a).name() + std::string("Vs") + typeid(*b).name();
			EntityPair pairKey = std::make_pair(a->GetOwner()->GetId(), b->GetOwner()->GetId());

			bool notContains = false; /// mapがペアを持っていないかどうか
			if (!collisionFrameMap.contains(pairKey)) {
				/// 逆順のないかチェック
				pairKey = std::make_pair(b->GetOwner()->GetId(), a->GetOwner()->GetId());
				if (!collisionFrameMap.contains(pairKey)) {
					notContains = true;
				}
			}

			if (notContains) {
				++collisionFrameMap[pairKey]; // 衝突計算をしたフレームを記録

				/// 衝突計算を行う
				CollisionPair pair(a->GetOwner(), b->GetOwner());
				auto it = collisionCheckMap_.find(collisionType);
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



	/// call back関数の実行
	for (auto& pair : collisionPairs_) {
		GameEntity* entityA = pair.first;
		GameEntity* entityB = pair.second;
		/// 衝突している場合の処理
		if (entityA && entityB) {
			/// 衝突イベントの実行
			std::array<Script*, 2> scripts;
			std::array<GameEntity*, 2> entities = { entityA, entityB };
			scripts[0] = entityA->GetComponent<Script>();
			scripts[1] = entityB->GetComponent<Script>();

			for (size_t i = 0; i < 2; i++) {
				if (!scripts[i]) {
					continue; // スクリプトがない場合はスキップ
				}

				auto& data = scripts[i]->GetScriptDataList();
				for (auto& script : data) {

					MonoObject* exc = nullptr;

					/// 引数の準備
					void* params[1];
					params[0] = entities[(i + 1) % 2]; /// 衝突しているもう一方のオブジェクトを渡す

					/// 関数の実行
					MonoObject* safeObj = mono_gchandle_get_target(script.gcHandle);
					mono_runtime_invoke(script.collisionEventMethods[0], safeObj, params, &exc);

					/// 例外が発生した場合の処理
					if (exc) {
						MonoString* monoStr = mono_object_to_string(exc, nullptr);
						if (monoStr) {
							char* message = mono_string_to_utf8(monoStr);
							Console::Log(std::string("Mono Exception: ") + message);
							mono_free(message);
						} else {
							Console::Log("Mono Exception occurred, but message is null.");
						}
					}

				}
			}



		}
	}



	collisionPairs_.clear(); ///< 衝突ペアをクリア
}


bool CheckMethod::CollisionCheckSphereVsSphere(SphereCollider* _s1, SphereCollider* _s2) {
	if (!_s1 || !_s2) {
		return false; // 型が一致しない場合は衝突なし
	}

	GameEntity* e1 = _s1->GetOwner();
	GameEntity* e2 = _s2->GetOwner();

	float distance = (e1->GetPosition() - e2->GetPosition()).Len();
	return distance <= (_s1->GetRadius() + _s2->GetRadius());
}

bool CheckMethod::CollisionCheckSphereVsBox(SphereCollider* _s, BoxCollider* _b) {
	if (!_s || !_b) {
		return false; // 型が一致しない場合は衝突なし
	}
	GameEntity* e1 = _s->GetOwner();
	GameEntity* e2 = _b->GetOwner();
	return CollisionCheck::CubeVsSphere(
		e2->GetPosition(), _b->GetSize(),
		e1->GetPosition(), _s->GetRadius()
	);
}

bool CheckMethod::CollisionCheckBoxVsBox(BoxCollider* _b1, BoxCollider* _b2) {
	if (!_b1 || !_b2) {
		return false; // 型が一致しない場合は衝突なし
	}
	GameEntity* e1 = _b1->GetOwner();
	GameEntity* e2 = _b2->GetOwner();
	return CollisionCheck::CubeVsCube(
		e1->GetPosition(), _b1->GetSize(),
		e2->GetPosition(), _b2->GetSize()
	);
}

