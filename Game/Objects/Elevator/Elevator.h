#pragma once

#include <GameObjectManager.h>

class Wire;

class Elevator final : public BaseGameObject {
public:

	Elevator() {
		CreateTag(this);
	}
	~Elevator() {}

	void Initialize() override;
	void Update() override;
	void Draw() override;


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
	float constantSpeed_ = 0.25f;

};