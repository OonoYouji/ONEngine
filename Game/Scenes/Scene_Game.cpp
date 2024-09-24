#include "Scene_Game.h"

#include "Player/Player.h"


/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {
	
	(new Player)->Initialize();
	(new Player)->Initialize();

	/*for(uint32_t i = 0u; i < 120u; ++i) {
		(new Player)->Initialize();
	}*/

}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {

}


