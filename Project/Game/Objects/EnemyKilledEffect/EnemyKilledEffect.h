#pragma once

#include "GameObjectManager/BaseGameObject.h"

#include "CustomMath/Flag.h"

class EnemyKilledEffect : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	EnemyKilledEffect(const std::vector<BaseGameObject*>& _objects);
	~EnemyKilledEffect();

	void Initialize() override;
	void Update()     override;

	void AddVariables();
	void ApplyVariables();

	const Flag& GetIsFinish() const { return isFinish_; }

private:

	/// ===================================================
	/// private : objects
	/// ===================================================


	float maxTime_;
	float currentTime_;

	std::vector<BaseGameObject*> objectVector_;
	Flag isFinish_;

};
