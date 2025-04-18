#include "EffectUpdateSystem.h"

/// std
#include <list>

/// engine
#include "Engine/Core/Utility/Time/Time.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Component.h"

void EffectUpdateSystem::Update(EntityComponentSystem* _pEntityComponentSystem) {

	/// エフェクトコンポーネントを持つエンティティを取得
	std::list<Effect*> effectList;
	for (auto& entity : _pEntityComponentSystem->GetEntities()) {
		auto effect = entity->GetComponent<Effect>();
		if (effect) {
			effectList.push_back(effect);
		}
	}

	/// エフェクトの更新処理
	for (auto& effect : effectList) {

		/// エフェクトの要素を更新
		for (auto& element : effect->elements_) {
			element.transform.position += element.velocity; // 位置の更新
			element.lifeTime -= Time::DeltaTime(); // 寿命の減少(仮に1フレーム0.016秒とする)
			if (element.lifeTime <= 0.0f) {
				// エフェクトの寿命が尽きた場合、要素を削除する処理を追加
			}

			element.transform.Update();
		}

	}

}
