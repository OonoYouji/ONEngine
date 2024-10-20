#pragma once

/// components
#include "ComponentManager/ParticleSystem/ParticleSystem.h"

/// base class
#include "GameObjectManager/BaseGameObject.h"

class TitlePlayerAnimator final : public BaseGameObject {

	struct WindAnimationData {
		float time  = 0.0f;
		float speed = 1.0f;
	};

	struct ParticleData {
		float time    = 0.0f;
		float speed   = 1.0f;
		float radius  = 1.0f;
		float maxPosY = 0.0f;
		Vec3 rotate{};
		Vec3 scale{};
	};

public:

	TitlePlayerAnimator() { CreateTag(this); }
	~TitlePlayerAnimator() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;


	ParticleSystem* GetParticleSystem() const { return particleSystem_; }


	void SetBasePosition(const Vec3& _basePosition);

	void SetIsSpinUpdate(bool isSpinUpdate);

	void SetParticleUseRotate(bool _useRotate);

	void SetUseShadow(bool _useShadow);

private:

	Vec3 basePosition_;

	float animationTime_;
	float animationSpeed_;
	float animationAmplitude_;


	bool isSpinUpdate_ = true;
	bool isSpin_       = false;
	float spinTime_    = 0.0f;
	float spinSpeed_   = 1.0f;

	std::vector<class Wind*>       windArray_;
	std::vector<WindAnimationData> windAnimationDataArray_;
	std::vector<ParticleData>      particleDataArray_;
	ParticleSystem*                particleSystem_ = nullptr;

	Mat4 matRotate_;
	bool useRotate_;

	bool useShadow_ = true; /// initializeの前にセットしないと意味なし
};
