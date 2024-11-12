#pragma once

#include "GameObjectManager/BaseGameObject.h"

class Boss : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Boss();
	~Boss();

	void Initialize() override;
	void Update()     override;

	void OnCollisionEnter(BaseGameObject* const _collision) override;

	void EndBattel();


	void SetRailCamera(class RailCamera* _railCamera);

	void SubHP(float _value);

	float GetHP() const { return hp_; }

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class MeshRenderer* meshRenderer_ = nullptr;


	/// other clss pointer
	class RailCamera* pRailCamera_ = nullptr;

	bool isBattelStarted_ = false;
	float battelTime_ = 10.0f;

	float hp_ = 20.0f;

};
