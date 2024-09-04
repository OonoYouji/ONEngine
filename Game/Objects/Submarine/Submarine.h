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

	void Debug() override;

private:

	Model* model_ = nullptr;

	float distance_ = 0.0f;
	Vec3 mouseRayPosition_ = {};


};