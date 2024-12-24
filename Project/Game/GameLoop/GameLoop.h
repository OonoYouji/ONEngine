#pragma once

/// engine
#include "Engine/GameFramework/IGameLoop/IGameLoop.h"

class GameLoop : public IGameLoop {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================
	
	GameLoop();
	~GameLoop();

	void Initialize() override;
	void Update()     override;
	void Draw()       override;
	void Finalize()   override;
	
	bool GetIsEnd()   override;
	
private:
	
	/// ===================================================
	/// private : objects
	/// ===================================================

	bool isEnd_ = false;
};

