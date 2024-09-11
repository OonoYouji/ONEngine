#pragma once

#include <memory>

#include <GameObjectManager.h>
#include <AudioManager.h>

class GameOperationUI final : public BaseGameObject {
public:

	GameOperationUI() { CreateTag(this); }
	~GameOperationUI() {}

	void Initialize() override;
	void Update() override;
	void Draw() override;

	void Debug() override;
private:
	Model* paper_ = nullptr;
	Model* binder_ = nullptr;
	Material paperMaterial_;
	Vec4 paperColor_;

	float lerpTime_ = 0.0f;
	float maxLerpTime_ = 0.0f;


	bool isFrontBinder_ = false;

	class AudioSource* sousa_ = nullptr;

};