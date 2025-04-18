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
			UpdateElement(&element);
		}

	}

}

void EffectUpdateSystem::UpdateElement(Effect::Element* _element) {
	_element->transform.position += _element->velocity; // 位置の更新
	_element->lifeTime -= Time::DeltaTime();
	if (_element->lifeTime <= 0.0f) {
		// エフェクトの寿命が尽きた場合、要素を削除する処理を追加
		return;
	}

	_element->transform.Update();
}
