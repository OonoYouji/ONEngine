#pragma once

#include "GameObjectManager/BaseGameObject.h"

#include "CustomMath/Flag.h"

class GameStartEffect : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	GameStartEffect(std::vector<BaseGameObject*>& _gameObjectVector);
	~GameStartEffect();

	void Initialize() override;
	void Update()     override;

	void AddVariables();
	void ApplyVariables();


	void StartGame();

	void SetMaxEffectTime(float _time);
	float GetMaxEffectTime() const { return maxEffectTime_; }

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class AudioSource* se_;
	std::vector<BaseGameObject*> gameObjectVector_;
	class Enemy* enemy_;

	float transitionTime_ = 2.0f;

	float maxEffectTime_;
	float currentEffectTime_;

	float seTriggerTime_;

	Flag isPlaySE_;
	Flag isFinish_;
	Flag isRestart_;

};
