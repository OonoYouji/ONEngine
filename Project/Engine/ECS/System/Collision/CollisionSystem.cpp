#include "CollisionSystem.h"

/// std
#include <unordered_map>
#include <utility>

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/Script/MonoScriptEngine.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Collision/BoxCollider.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Collision/SphereCollider.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Collision/CollisionCheck/CollisionCheck.h"


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
			_pair.second->GetComponent<SphereCollider>(),
			_pair.first->GetComponent<BoxCollider>()
		);
		};

	collisionCheckMap_[boxCompName + "Vs" + boxCompName] = [](const CollisionPair& _pair) -> bool {
		return CheckMethod::CollisionCheckBoxVsBox(
			_pair.first->GetComponent<BoxCollider>(),
			_pair.second->GetComponent<BoxCollider>()
		);
		};

}

void CollisionSystem::OutsideOfRuntimeUpdate(ECSGroup* _ecs) {

	/// SphereColliderのGizmo描画
	ComponentArray<SphereCollider>* sphereColliderArray = _ecs->GetComponentArray<SphereCollider>();
	if (sphereColliderArray) {
		for (auto& sphereCollider : sphereColliderArray->GetUsedComponents()) {
			if (sphereCollider) {
				if (GameEntity* owner = sphereCollider->GetOwner()) {
					Gizmo::DrawWireSphere(owner->GetPosition(), sphereCollider->GetRadius(), Color::kRed);
				}
			}
		}
	}

	/// BoxColliderのGizmo描画
	ComponentArray<BoxCollider>* boxColliderArray = _ecs->GetComponentArray<BoxCollider>();
	if (boxColliderArray) {
		for (auto& boxCollider : boxColliderArray->GetUsedComponents()) {
			if (boxCollider) {
				if (GameEntity* owner = boxCollider->GetOwner()) {
					Gizmo::DrawWireCube(owner->GetPosition(), boxCollider->GetSize(), Color::kRed);
				}
			}
		}
	}

}

void CollisionSystem::RuntimeUpdate(ECSGroup* _ecs) {

	enterPairs_.clear();
	stayPairs_.clear();
	exitPairs_.clear();

	/// 全てのコライダーを取得
	ComponentArray<SphereCollider>* sphereColliderArray = _ecs->GetComponentArray<SphereCollider>();
	ComponentArray<BoxCollider>* boxColliderArray = _ecs->GetComponentArray<BoxCollider>();

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


	/// 衝突計算をしたフレームを記録するマップ
	using EntityIdPair = std::pair<int, int>;
	std::unordered_map<EntityIdPair, int> collisionFrameMap;

	/// 衝突判定
	std::string collisionType = "";
	for (auto& a : colliders) {
		for (auto& b : colliders) {
			/// 同じオブジェクト同士の衝突は無視
			if (a == b) {
				continue;
			}

			/// このフレームないで衝突計算をしているかチェック
			collisionType = typeid(*a).name() + std::string("Vs") + typeid(*b).name();
			EntityIdPair pairKey = std::make_pair(a->GetOwner()->GetId(), b->GetOwner()->GetId());

			/*
			* --- 衝突判定の流れ ---
			* 1, このフレームですでに衝突判定をとっているかチェック
			* 2, まだなら衝突判定を計算
			* 3, 衝突しているなら collisionPairs_ にペアを追加
			* 4, 衝突していないなら前 collisionPairs_ にペアがあれば削除し、releasePairs_にペアを追加
			* 5, call back関数の実行
			*/


			/// mapがペアを持っていないかどうか
			bool collisionFrameMapContains = false;
			if (!collisionFrameMap.contains(pairKey)) {
				/// 逆順でないかチェック
				pairKey = std::make_pair(b->GetOwner()->GetId(), a->GetOwner()->GetId());
				if (!collisionFrameMap.contains(pairKey)) {
					collisionFrameMapContains = true;
				}
			}

			/// このフレームで衝突判定をしている場合はスキップする
			if (!collisionFrameMapContains) {
				continue;
			}

			/// 衝突計算をしたフレームを記録
			++collisionFrameMap[pairKey];


			/// 衝突計算の関数を取得
			CollisionPair pair(a->GetOwner(), b->GetOwner());
			auto collisionCheckItr = collisionCheckMap_.find(collisionType);
			if (collisionCheckItr == collisionCheckMap_.end()) {
				continue;
			}

			/// 衝突計算を行う
			bool isCollided = collisionCheckItr->second(pair);
			if (isCollided) {
				/// collidedPairs_にペアがすでに存在しているかチェック
				auto collisionPairItr = std::find_if(collidedPairs_.begin(), collidedPairs_.end(), [&pair](const CollisionPair& _p) {
					return (_p.first == pair.first && _p.second == pair.second)
						|| (_p.first == pair.second && _p.second == pair.first);
					});

				if (collisionPairItr != collidedPairs_.end()) {
					/// すでにペアが存在している場合は stayPairs_ に追加
					stayPairs_.emplace_back(pair);
				} else {
					/// 新たにペアが追加された場合は enterPairs_ に追加
					enterPairs_.emplace_back(pair);
				}

				/// 衝突している場合はペアを記録
				collidedPairs_.emplace_back(pair);

			} else {

				/// collisionPairs_からペアを削除
				auto collisionPairItr = std::remove_if(collidedPairs_.begin(), collidedPairs_.end(), [&pair](const CollisionPair& _p) {
					return (_p.first == pair.first && _p.second == pair.second)
						|| (_p.first == pair.second && _p.second == pair.first);
					});

				/// 削除するペアがあった場合は exitPairs_ に追加
				if (collisionPairItr != collidedPairs_.end()) {
					exitPairs_.emplace_back(pair);
				}

				collidedPairs_.erase(collisionPairItr, collidedPairs_.end());
			}

		}
	}


	/// 各コールバック関数の実行
	const std::string& ecsGroupName = _ecs->GetGroupName();
	CallEnterFunc(ecsGroupName);
	CallStayFunc(ecsGroupName);
	CallExitFunc(ecsGroupName);

}

void CollisionSystem::CallEnterFunc(const std::string& _ecsGroupName) {
	MonoScriptEngine* monoEngine = MonoScriptEngine::GetInstance();

	for (auto& pair : enterPairs_) {
		GameEntity* entityA = pair.first;
		GameEntity* entityB = pair.second;

		/// ポインタが有効でないならスキップ
		if (!entityA || !entityB) {
			continue;
		}

		/// 衝突イベントの実行
		std::array<GameEntity*, 2> entities = { entityA, entityB };
		std::array<Script*, 2>     scripts = { entityA->GetComponent<Script>(), entityB->GetComponent<Script>() };

		for (size_t i = 0; i < 2; i++) {
			if (!scripts[i]) {
				continue;
			}

			auto& data = scripts[i]->GetScriptDataList();
			for (auto& script : data) {
				MonoObject* exc = nullptr;

				/// 引数の準備
				void* params[1];
				params[0] = entities[(i + 1) % 2]; /// 衝突しているもう一方のオブジェクトを渡す

				MonoObject* monoBehavior = monoEngine->GetMonoBehaviorFromCS(_ecsGroupName, scripts[i]->GetOwner()->GetId(), script.scriptName);
				if (!script.collisionEventMethods[0]) {
					script.collisionEventMethods[0] = monoEngine->GetMethodFromCS(script.scriptName, "OnCollisionEnter", 1);
				}

				mono_runtime_invoke(script.collisionEventMethods[0], monoBehavior, params, &exc);


				Console::Log("Collision Enter Event Invoked");

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

void CollisionSystem::CallStayFunc(const std::string& _ecsGroupName) {
	MonoScriptEngine* monoEngine = MonoScriptEngine::GetInstance();

	for (auto& pair : stayPairs_) {
		GameEntity* entityA = pair.first;
		GameEntity* entityB = pair.second;

		/// ポインタが有効でないならスキップ
		if (!entityA || !entityB) {
			continue;
		}

		/// 衝突イベントの実行
		std::array<GameEntity*, 2> entities = { entityA, entityB };
		std::array<Script*, 2>     scripts = { entityA->GetComponent<Script>(), entityB->GetComponent<Script>() };

		for (size_t i = 0; i < 2; i++) {
			if (!scripts[i]) {
				continue;
			}

			auto& data = scripts[i]->GetScriptDataList();
			for (auto& script : data) {
				MonoObject* exc = nullptr;

				/// 引数の準備
				void* params[1];
				params[0] = entities[(i + 1) % 2]; /// 衝突しているもう一方のオブジェクトを渡す

				MonoObject* monoBehavior = monoEngine->GetMonoBehaviorFromCS(_ecsGroupName, scripts[i]->GetOwner()->GetId(), script.scriptName);
				if (!script.collisionEventMethods[1]) {
					script.collisionEventMethods[1] = monoEngine->GetMethodFromCS(script.scriptName, "OnCollisionStay", 1);
				}

				mono_runtime_invoke(script.collisionEventMethods[1], monoBehavior, params, &exc);

				Console::Log("Collision Stay Event Invoked");

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

void CollisionSystem::CallExitFunc(const std::string& _ecsGroupName) {
	MonoScriptEngine* monoEngine = MonoScriptEngine::GetInstance();

	for (auto& pair : exitPairs_) {
		GameEntity* entityA = pair.first;
		GameEntity* entityB = pair.second;

		/// ポインタが有効でないならスキップ
		if (!entityA || !entityB) {
			continue;
		}

		/// 衝突イベントの実行
		std::array<GameEntity*, 2> entities = { entityA, entityB };
		std::array<Script*, 2>     scripts = { entityA->GetComponent<Script>(), entityB->GetComponent<Script>() };

		for (size_t i = 0; i < 2; i++) {
			if (!scripts[i]) {
				continue;
			}

			auto& data = scripts[i]->GetScriptDataList();
			for (auto& script : data) {
				MonoObject* exc = nullptr;

				/// 引数の準備
				void* params[1];
				params[0] = entities[(i + 1) % 2]; /// 衝突しているもう一方のオブジェクトを渡す


				MonoObject* monoBehavior = monoEngine->GetMonoBehaviorFromCS(_ecsGroupName, scripts[i]->GetOwner()->GetId(), script.scriptName);
				if (!script.collisionEventMethods[2]) {
					script.collisionEventMethods[2] = monoEngine->GetMethodFromCS(script.scriptName, "OnCollisionExit", 1);
				}

				mono_runtime_invoke(script.collisionEventMethods[2], monoBehavior, params, &exc);


				Console::Log("Collision Exit Event Invoked");

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

