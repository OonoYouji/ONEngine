#pragma once

/// engine
#include <Engine/Entity/Interface/IEntity.h>

/// ///////////////////////////////////////////////////
/// GameController
/// ///////////////////////////////////////////////////
class GameController : public IEntity {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	GameController();
	~GameController();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class Camera*       camera_;
	class Player*       player_;
	class BlockManager* blockManager_;


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================



};

