#include "EffectUpdateSystem.h"

/// std
#include <list>
#include <numbers>

/// engine
#include "Engine/Core/Utility/Time/Time.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Entity/Camera/Camera.h"
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

	mainCamera_ = _pEntityComponentSystem->GetMainCamera();
	matBillboard_ = Matrix4x4::MakeRotate(mainCamera_->GetWorldRotate());
	//matBillboard_.m[3][0] = 0.0f;
	//matBillboard_.m[3][1] = 0.0f;
	//matBillboard_.m[3][2] = 0.0f;


	/// エフェクトの更新処理
	for (auto& effect : effectList) {

		/// エフェクトの要素を更新
		for (auto& element : effect->elements_) {
			UpdateElement(effect, &element);
		}


		///!< エフェクトを出現するまでの
		if (effect->isCreateParticle_) {

			switch (effect->emitType_) {
				/// ----------------------------------------
				/// 距離で指定する場合の処理
				/// ----------------------------------------
			case Effect::EmitType::Distance:
			{

				Effect::DistanceEmitData& data = effect->distanceEmitData_;
				data.currentPosition = data.nextPosition;
				data.nextPosition = effect->owner_->GetWorldPosition();

				data.moveLength = Vec3::Length(data.nextPosition - data.currentPosition);
				data.emitInterval -= data.moveLength;

				if (data.emitInterval <= 0.0f) {
					data.emitInterval = data.emitDistance;

					/// エフェクトの要素を生成
					for (size_t i = 0; i < effect->emitInstanceCount_; i++) {
						if (effect->elements_.size() < effect->maxEffectCount_) {
							effect->CreateElement(
								effect->GetOwner()->GetWorldPosition(),
								Vec3::kUp,
								effect->emittedElementColor_
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
								effect->GetOwner()->GetLocalPosition(),
								effect->emittedElementColor_
							);
						}
					}
				}

			}
			break;
			}

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

void EffectUpdateSystem::UpdateElement(Effect* _effect, Effect::Element* _element) {

	if (_effect->elementUpdateFunc_) {
		_effect->elementUpdateFunc_(_element);
	}

	_element->transform.position += _element->velocity * Time::DeltaTime();
	_element->lifeTime -= Time::DeltaTime();
	if (_element->lifeTime <= 0.0f) {
		return;
	}

	if (_effect->useBillboard_) {
		Matrix4x4&& matScale = Matrix4x4::MakeScale(_element->transform.scale);
		Matrix4x4&& matTranslate = Matrix4x4::MakeTranslate(_element->transform.position);

		_element->transform.matWorld = matScale * matBillboard_ * matTranslate;

	} else {
		_element->transform.Update();
	}
}
