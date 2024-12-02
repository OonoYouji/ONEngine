#pragma once

#include "Core/Framework.h"

class GameFrameWork final : public Framework {
public:
	GameFrameWork() {}
	~GameFrameWork() {}

	void Initialize() override;
	void Update()     override;
	void Draw()       override;
	void Finalize()   override;

private:

	class SceneManager* pSceneManager_ = nullptr;
};