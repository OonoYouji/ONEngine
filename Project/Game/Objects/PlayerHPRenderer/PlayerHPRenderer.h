#pragma once

#include "GameObjectManager/BaseGameObject.h"

class PlayerHPRenderer : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerHPRenderer();
	~PlayerHPRenderer();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;


	/// <summary>
	/// プレイヤーのポインタをセットする
	/// </summary>
	/// <param name="_player"> : プレイヤーのポインタ </param>
	void SetPlayer(class Player* _player);

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
	Vec2                  uvAnchor_       = { 0.0f, 0.5f };
	Vec3                  maxScale_       = { 0.0f, 0.0f, 0.0f };

	/// other class
	class Player* pPlayer_ = nullptr;

	/// 
	class PlayerHPFrame* hpFrame_ = nullptr; /// 実体をここで作る


};
