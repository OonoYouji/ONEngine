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

	void SetWire(Wire* wire);

private:

	Model* model_ = nullptr;

	/// このエレベーターが作成されたタイミング時のワイヤーへのポインタ
	Wire* wire_ = nullptr;

};