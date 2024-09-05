#pragma once

#include <GameObjectManager.h>

class Wire;

/// <summary>
/// ワイヤーを伝うエレベーター
/// </summary>
class Elevator final : public BaseGameObject {
public:

	Elevator() {
		CreateTag(this);
	}
	~Elevator() {}

	void Initialize() override;
	void Update() override;
	void Draw() override;

	void Debug() override;


	/// ---------------------------------------- ///
	///		not override methods
	/// ---------------------------------------- ///

	void SetWire(Wire* wire);

private:

	Model* model_ = nullptr;

	/// このエレベーターが作成されたタイミング時のワイヤーへのポインタ
	Wire* wire_ = nullptr;

	/// 上下の移動速度 : 秒単位
	float speed_ = 0.0f;
	/// スピードの固定値
	static float sConstantSpeed_;

};