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

		switch (effect->emitType_) {
			/// ----------------------------------------
			/// 距離で指定する場合の処理
			/// ----------------------------------------
		case Effect::EmitType::Distance:
		{

			Effect::DistanceEmitData& data = effect->distanceEmitData_;
			data.currentPosition = data.nextPosition;
			data.nextPosition = effect->owner_->GetTransform()->position;
			data.moveLength = Vec3::Length(data.nextPosition - data.currentPosition);
			data.emitInterval -= data.moveLength;

			if (data.emitInterval <= 0.0f) {
				data.emitInterval = data.emitDistance;

				/// エフェクトの要素を生成
				for (size_t i = 0; i < effect->emitInstanceCount_; i++) {
					if (effect->elements_.size() < effect->maxEffectCount_) {
						effect->CreateElement(
							effect->GetOwner()->GetPosition(),
							Vec3::kUp,
							Vec4::kWhite
						);
					}
				}
			}

		}
		break;
		/// ----------------------------------------
		/// 時間で指定する場合の処理
		/// ----------------------------------------
		case Effect::EmitType::Time:
		{
			Effect::TimeEmitData& data = effect->timeEmitData_;
			data.emitInterval -= Time::DeltaTime();
			if (data.emitInterval <= 0.0f) {
				data.emitInterval = data.emitTime;

				/// エフェクトの要素を生成
				for (size_t i = 0; i < effect->emitInstanceCount_; i++) {
					if (effect->elements_.size() < effect->maxEffectCount_) {
						effect->CreateElement(
							effect->GetOwner()->GetPosition(),
							Vec4::kWhite
						);
					}
				}
			}

		}
		break;
		}

	}


	/// -------------------------------------
	/// エフェクトの要素を削除
	/// -------------------------------------

	for (auto& effect : effectList) {
		if (effect->elements_.empty()) {
			continue;
		}

		for (size_t i = 0; i < effect->elements_.size(); i++) {
			if (effect->elements_[i].lifeTime <= 0.0f) {
				effect->RemoveElement(i);
				i--;
			}
		}
	}

}

void EffectUpdateSystem::UpdateElement(Effect::Element* _element) {
	_element->transform.position += _element->velocity * Time::DeltaTime();
	_element->lifeTime -= Time::DeltaTime();
	if (_element->lifeTime <= 0.0f) {
		return;
	}

	_element->transform.Update();
}
