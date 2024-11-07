#pragma once

/// std
#include <cstdint>

#include "GameObjectManager/BaseGameObject.h"

class ScoreBoard : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	ScoreBoard();
	~ScoreBoard();

	void Initialize() override;
	void Update()     override;


	/// <summary>
	/// jsonに保存した値の適用
	/// </summary>
	void ApplyVariables();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================


	uint32_t currentScore_;


	class SpriteRenderer* spriteRenderer_ = nullptr;

};
