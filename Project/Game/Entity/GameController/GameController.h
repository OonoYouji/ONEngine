#pragma once

/// std
#include <string>

/// engine
#include <Engine/ECS/Entity/Interface/IEntity.h>

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


	/// @brief ステージを読み込む
	/// @param _stageFilePath 
	void LoadStage(const std::string& _stageFilePath);

	/// @brief Playerの移動処理
	void MovePlayer();

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

