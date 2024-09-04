#pragma once

#include <GameObjectManager.h>


/// <summary>
/// 潜水艦
/// </summary>
class Submarine final : public BaseGameObject {
public:

	Submarine() {
		CreateTag(this);
	}
	~Submarine() {}


	void Initialize() override;
	void Update() override;

	void Draw() override;

private:

	Model* model_ = nullptr;

};