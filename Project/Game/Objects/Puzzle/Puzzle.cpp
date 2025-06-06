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

	float& toPlayerRadius = variables_->Get<float>("toPlayerRadius");


	Gizmo::DrawWireSphere(
		GetPosition(), toPlayerRadius, Vector4(1.0f, 0.0f, 0.0f, 1.0f)
	);

	MeshRenderer* meshRenderer = GetComponent<MeshRenderer>();
	if (meshRenderer) {
		meshRenderer->SetColor(Color::kWhite);
	}

	if (CollisionCheck::SphereVsSphere(
		player_->GetPosition(), 1.0f,
		GetPosition(), toPlayerRadius)) {

		meshRenderer->SetColor(Color::kRed);
	}

}
