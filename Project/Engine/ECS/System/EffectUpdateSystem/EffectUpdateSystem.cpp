#include "EffectUpdateSystem.h"

/// std
#include <list>
#include <numbers>

/// engine
#include "Engine/Core/Utility/Time/Time.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"

void EffectUpdateSystem::RuntimeUpdate(ECSGroup* _ecs) {

	ComponentArray<Effect>* effectArray = _ecs->GetComponentArray<Effect>();
	if (!effectArray || effectArray->GetUsedComponents().empty()) {
		return; // エフェクトのコンポーネント配列が存在しない場合は何もしない
	}

	mainCamera_ = _ecs->GetMainCamera();
	if (!mainCamera_) {
		Console::Log("EffectUpdateSystem::Update: main camera is null");
		return;
	}

	if (GameEntity* entity = mainCamera_->GetOwner()) {
		matBillboard_ = Matrix4x4::MakeRotate(entity->GetRotate());
	}
	//matBillboard_.m[3][0] = 0.0f;
	//matBillboard_.m[3][1] = 0.0f;
	//matBillboard_.m[3][2] = 0.0f;


	/// エフェクトの更新処理
	for (auto& effect : effectArray->GetUsedComponents()) {

		/// エフェクトの要素を更新
		for (auto& element : effect->elements_) {
			UpdateElement(effect, &element);
		}

		/// オブジェクトのアクティブ状態を確認
		/// アクティブじゃなければ新規生成しない
		if (!effect->GetOwner()->GetActive()) {
			continue;
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
				data.nextPosition = effect->owner_->GetPosition();

				data.moveLength = Vec3::Length(data.nextPosition - data.currentPosition);
				data.emitInterval -= data.moveLength;

				if (data.emitInterval <= 0.0f) {
					data.emitInterval = data.emitDistance;

					/// エフェクトの要素を生成
					for (size_t i = 0; i < effect->emitInstanceCount_; i++) {
						if (effect->elements_.size() < effect->maxEffectCount_) {

							std::pair<Vector3, Vector3> size = effect->mainModule_.GetValue<Vector3>(effect->mainModule_.sizeStartData_);
							std::pair<Vector3, Vector3> rotate = effect->mainModule_.GetValue<Vector3>(effect->mainModule_.rotateStartData_);
							std::pair<Color, Color> color = effect->mainModule_.GetValue<Color>(effect->mainModule_.colorStartData_);
							std::pair<float, float> speed = effect->mainModule_.GetValue<float>(effect->mainModule_.speedStartData_);
							Vector3 emitPos = effect->emitShape_.GetEmitPosition();

							effect->CreateElement(
								effect->GetOwner()->GetPosition() + effect->emitShape_.GetEmitPosition(),
								Random::Vector3(size.first, size.second),
								Random::Vector3(rotate.first, rotate.second),
								effect->emitShape_.GetEmitDirection(emitPos) * Random::Float(speed.first, speed.second),
								Random::Vector4(color.first, color.second)
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

							std::pair<Vector3, Vector3> size = effect->mainModule_.GetValue<Vector3>(effect->mainModule_.sizeStartData_);
							std::pair<Vector3, Vector3> rotate = effect->mainModule_.GetValue<Vector3>(effect->mainModule_.rotateStartData_);
							std::pair<Color, Color> color = effect->mainModule_.GetValue<Color>(effect->mainModule_.colorStartData_);
							std::pair<float, float> speed = effect->mainModule_.GetValue<float>(effect->mainModule_.speedStartData_);
							Vector3 emitPos = effect->emitShape_.GetEmitPosition();

							effect->CreateElement(
								effect->GetOwner()->GetPosition() + emitPos,
								Random::Vector3(size.first, size.second),
								Random::Vector3(rotate.first, rotate.second),
								effect->emitShape_.GetEmitDirection(emitPos) * Random::Float(speed.first, speed.second),
								Random::Vector4(color.first, color.second)
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

	for (auto& effect : effectArray->GetUsedComponents()) {
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

	if (_element->velocity != Vector3::kZero) {
		Console::Log("effect element velocity not zero");
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
