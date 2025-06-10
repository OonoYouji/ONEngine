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
	bool& isStartPuzzle = variables_->Get<bool>("isStartPuzzle");

	Gizmo::DrawWireSphere(
		GetPosition(), toPlayerRadius, Vector4(1.0f, 0.0f, 0.0f, 1.0f)
	);

	MeshRenderer* meshRenderer = GetComponent<MeshRenderer>();
	if (meshRenderer) {
		if (isStartPuzzle) {
			meshRenderer->SetColor(Color::kWhite);
		} else {
			meshRenderer->SetColor(Color::kRed);
		}
	}


	/// プレイヤーが一定範囲にいるかチェック
	if (CollisionCheck::SphereVsSphere(
		player_->GetPosition(), 1.0f,
		GetPosition(), toPlayerRadius)) {

		/// UIを表示



		/// 入力を受けてパズルを開始する
		if (Input::TriggerKey(DIK_E) || Input::TriggerGamepad(Gamepad::A)) {
			isStartPuzzle = !isStartPuzzle;
		}

	}

}
