#pragma once

#include <BaseScene.h>

class Scene_Result final : public BaseScene {
public:
	Scene_Result() {}
	~Scene_Result();

	void Initialize() override;
	void Update() override;
	void Draw() override {};
private:
	class AudioSource* bgm_ = nullptr;
};