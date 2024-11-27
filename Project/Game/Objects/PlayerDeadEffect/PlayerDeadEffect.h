#pragma once

#include "GameObjectManager/BaseGameObject.h"

#include "CustomMath/Flag.h"

class PlayerDeadEffect : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerDeadEffect(std::vector<BaseGameObject*>& _objects);
	~PlayerDeadEffect();

	void Initialize() override;
	void Update()     override;


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
