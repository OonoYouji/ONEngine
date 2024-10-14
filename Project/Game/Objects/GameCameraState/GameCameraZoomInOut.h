#pragma once

/// objects
#include "GameObjectManager/BaseGameObject.h"
#include "Objects/Camera/GameCamera.h"


/// ===================================================
/// 前方宣言
/// ===================================================

class Player;
/// ===================================================
/// ゲームカメラのズームを行うクラス
/// ===================================================
class GameCameraZoomInOut final : public BaseGameObject {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	GameCameraZoomInOut() { CreateTag(this); }
	~GameCameraZoomInOut() {}

	void Initialize() override;
	void Update()     override;
	void LastUpdate() override;
	void Debug()      override;

	//振る舞い関数
	void RootInit();
	void RootUpdate();
	void  ZoomInInit();
	void  ZoomInUpdate();
	void  ZoomOutInit();
	void  ZoomOutUpdate();
	void  BehaviorManagement();

	//ズーム切り替え
	void SetBehaviorZoomIn();
	void SetBehaviorZoomOut();

	void SetGameCamera(GameCamera* _gameCamera);
	void SetPlayer(Player* _player);
private:
	enum class Behavior {
		kRoot,
		kZoomIn,
		kZoomOut,
	};
private:
	/// ===================================================
	/// private : objects
	/// ===================================================
	float easeT_;
	float easeTMax_;
	float zoomOutMax_;
	float zoomInMax_;
	//// ふるまい
	Behavior behavior_;
	//振る舞いリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;


	/// other class pointer
	Player* pPlayer_ = nullptr;
	GameCamera* pGameCamera_ = nullptr;
};