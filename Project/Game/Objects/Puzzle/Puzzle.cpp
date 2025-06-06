#include "Puzzle.h"

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/ECS/Component/Component.h"

/// game
#include "Game/Entity/Player/Player.h"


void Puzzle::Initialize() {
	/// プレイヤーを探索してポインタを保持しておく
	player_ = pEntityComponentSystem_->FindEntity<Player>();
	if (!player_) {
		Console::Log("player not found");;
		return;
	}

}

void Puzzle::Update() {

}
