#include "ParticleEmitter.h"

/// engine
#include <imgui.h>
#include "FrameManager/Time.h"
#include "GameObjectManager/BaseGameObject.h"

/// components
#include "ParticleSystem.h"



void ParticleEmitter::Initialize(std::vector<std::unique_ptr<class Particle>>* _particleArray, ParticleSystem* _particleSystem) {

	/// other pointer
	pParticleArray_  = _particleArray;
	pParticleSystem_ = _particleSystem;


	/// 出現する形状と方法
	emissionShape_ = static_cast<int32_t>(EMISSION_SHAPE::BOX);
	emissionType_  = static_cast<int32_t>(EMISSION_TYEP::TIME);

	/// particle emission time
	rateOverTime_     = 1.0f;
	rateOverDistance_ = 1.0f;

	currentTime_      = rateOverTime_;
	currentDistance_  = rateOverDistance_;


	/// particle emission count
	emissionCount_ = 1u;
	currentParticleCount_ = 0u;
	maxParticleCount_ = pParticleSystem_->GetKMaxParticleNum();

	/// particle emission burst
	isBurst_ = false;

}

void ParticleEmitter::Update() {

	/// 時間を減らす
	currentTime_ -= Time::DeltaTime();

	/// パーティクルを発生させる
	if(currentTime_ < 0.0f) {
		/// 値のリセット
		currentTime_ = rateOverTime_;

		/// 発生
		Emit();
	}

}

void ParticleEmitter::Debug() {
	if(ImGui::TreeNodeEx("emitter")) {


		/// ---------------------------------------------------
		/// 出現する方法
		/// ---------------------------------------------------

		ImGui::SeparatorText("type");

		int currentTypeIndex = static_cast<int>(emissionType_);
		const char* emitterTypes[] = { "Time", "Distance" };
		if(ImGui::Combo("emitter type", &currentTypeIndex, emitterTypes, 2)) {
			emissionType_ = static_cast<int32_t>(currentTypeIndex);
		}

		if(emissionType_ == static_cast<int32_t>(EMISSION_TYEP::TIME)) {

			ImGui::SeparatorText("type time");

			ImGui::DragFloat("rate over time", &rateOverTime_, 0.05f);
			ImGui::DragFloat("current time",   &currentTime_,  0.05f);

		} else {

			ImGui::SeparatorText("type distance");

			ImGui::DragFloat("rate over distance", &rateOverDistance_, 0.05f);
			ImGui::DragFloat("current distance",   &currentDistance_,  0.05f);

		}



		/// ---------------------------------------------------
		/// emitterの形状
		/// ---------------------------------------------------

		ImGui::SeparatorText("shape");

		int currentShapeIndex       = static_cast<int>(emissionShape_);
		const char* emitterShapes[] = { "Box", "Sphere" };
		if(ImGui::Combo("emitter shape", &currentShapeIndex, emitterShapes, 2)) {
			emissionShape_ = static_cast<int32_t>(currentShapeIndex);
		}

		/// emitterの形状ごとに別のdebugをする
		if(emissionShape_ == static_cast<int32_t>(EMISSION_SHAPE::BOX)) {

			ImGui::SeparatorText("shape box");

			ImGui::DragFloat3("min", &min_.x, 0.01f);
			ImGui::DragFloat3("max", &max_.x, 0.01f);

		} else {

			ImGui::SeparatorText("shape sphere");

			ImGui::DragFloat("radius", &radius_, 0.01f);

		}


		ImGui::TreePop();
	}
}

void ParticleEmitter::Emit() {
	for(size_t i = 0; i < static_cast<size_t>(emissionCount_); ++i) {

		/// 現在のparticle配列の大きさと最大値を比べる
		size_t particleArraySize = pParticleArray_->size();
		if(particleArraySize < static_cast<size_t>(maxParticleCount_)) {

			pParticleArray_->push_back(std::make_unique<Particle>());
			pParticleArray_->back()->Initialize();
			pParticleArray_->back()->lifeTime_ = pParticleSystem_->GetParticleLifeTime();

		} else {
			/// ここに入るときはすでに最大値分配列を作成している

			/// 1, isAliveがfalseになっているオブジェクトを探す
			/// 2, 見つかったオブジェクトを使用してパーティクルを発生させる

			auto itr = std::find_if( pParticleArray_->begin(),  pParticleArray_->end(),
									[](const std::unique_ptr<Particle>& particle) {
				if(!particle->GetIsAlive()) {
					return true;
				}
				return false;
			});

			/// 見つからなかったらこれ以降もないので終了する
			if(itr ==  pParticleArray_->end()) {
				break;
			}

			(*itr)->GetTransform()->position = pParticleSystem_->GetOwner()->GetPosition();
			(*itr)->lifeTime_ = pParticleSystem_->GetParticleLifeTime();
			(*itr)->isAlive_ = true;

		}

	}
}
