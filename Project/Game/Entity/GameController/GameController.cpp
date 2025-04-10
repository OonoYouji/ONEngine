#include "GameController.h"

/// std
#include <numbers>

/// engine
#include <Engine/Entity/Collection/EntityCollection.h>
#include <Engine/Core/Utility/Input/Input.h>
#include "Engine/Entity/Camera/Camera.h"

/// user
#include "../Player/Player.h"
#include "../Block/Block.h"
#include "../EntityConfig/EntityConfig.h"


GameController::GameController() {}
GameController::~GameController() {}



void GameController::Initialize() {

	camera_ = pEntityCollection_->GenerateCamera();
	camera_->SetPosition(Vector3(0.0f, 20.0f, -25.0f));
	camera_->SetRotate(Vector3(std::numbers::pi_v<float> / 5.0f, 0.0f, 0.0f));
	pEntityCollection_->SetMainCamera(camera_);

	blockManager_ = pEntityCollection_->GenerateEntity<BlockManager>();
	player_       = pEntityCollection_->GenerateEntity<Player>();
}

void GameController::Update() {

	MovePlayer();

}



void GameController::LoadStage([[maybe_unused]] const std::string& _stageFilePath) {
	///< ステージデータの読み込み




}

void GameController::MovePlayer() {

	///< プレイヤーの移動処理、 移動方向の計算
	Address input{};

	if (Input::TriggerKey(DIK_W)) {
		input.row = 1;
		input.col = 0;
	}

	if (Input::TriggerKey(DIK_S)) {
		input.row = -1;
		input.col = 0;
	}

	if (Input::TriggerKey(DIK_A)) {
		input.row = 0;
		input.col = -1;
	}

	if (Input::TriggerKey(DIK_D)) {
		input.row = 0;
		input.col = 1;
	}


	///!< 入力がない場合
	if (input.row == 0 && input.col == 0) {
		return;
	}

	///< プレイヤーの移動
	const Address&       current     = player_->GetCurrentAddress();
	std::pair<bool, int> checkResult = blockManager_->CheckAddress(current + input);

	///!< ステージ外
	if (!checkResult.first) {
		return;
	}

	///!< プレイヤーとブロックのタイプが一致していない場合
	if (player_->GetType() != checkResult.second) {
		return;
	}

	///< 移動先のアドレスを指定する
	player_->Move(current + input);


}
