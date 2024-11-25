#pragma once

#include "GameObjectManager/BaseGameObject.h"

class EnemyHPRenderer : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	EnemyHPRenderer();
	~EnemyHPRenderer();

	void Initialize() override;
	void Update()     override;

	void SetEnemy(class Enemy* _enemy);

private:

	/// <summary>
	/// 値の適用
	/// </summary>
	void ApplyVariables();

	/// <summary>
	/// 値のセット
	/// </summary>
	void SetVariables();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	/// sprtie component
	class SpriteRenderer* spriteRenderer_ = nullptr;
	Vec2                  uvAnchor_ = { 0.0f, 0.5f };
	Vec3                  maxScale_ = { 0.0f, 0.0f, 0.0f };

	/// other class
	class Enemy* pEnemy_ = nullptr;

	/// 
	class EnemyHPFrame* hpFrame_ = nullptr; /// 実体をここで作る


};
