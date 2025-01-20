#pragma once

#include "Engine/Scene/IScene.h"


/// ===================================================
/// ゲームのメインシーン
/// ===================================================
class GameScene final : public IScene {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================
	
	GameScene();
	~GameScene();
	
	void Initialize() override;
	void Update()     override;
	void Draw()       override;


private:

};

