#pragma once

/// objects
#include<GraphicManager/Light/DirectionalLight.h>
#include "GameObjectManager/BaseGameObject.h"
#include "Objects/Camera/GameCamera.h"


/// ===================================================
/// 前方宣言
/// ===================================================

class Player;

enum class CameraBehavior {
	kRoot,
	kZoomIn,
	kZoomOut,
};


/// ===================================================
/// ゲームカメラのズームを行うクラス
/// ===================================================
class GameCameraState final : public BaseGameObject {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	GameCameraState() { CreateTag(this); }
	~GameCameraState() {}

	void Initialize() override;
	void Update()     override;
	void LastUpdate() override;
	void Debug()      override;

	//振る舞い関数
	void RootInit();
	void RootUpdate();
	void ZoomInInit();
	void ZoomInUpdate();
	void ZoomOutInit();
	void ZoomOutUpdate();

	void  BehaviorManagement();

	//ズーム切り替え
	void SetBehaviorZoomIn();
	void SetBehaviorZoomOut();
	void SetBehaviorZoomRoot();

	void SetGameCamera(GameCamera* _gameCamera);
	void SetPlayer(Player* _player);
	void SetDirectionalLight(DirectionalLight* _directionalLight);
private:
	
private:
	/// ===================================================
	/// private : objects
	/// ===================================================
	float easeT_;
	float easeTMax_;
	float zoomOutMax_;
	float zoomInMax_;
	float zoomRoot_; /// 通常人のzoom量

	float currentZoomValue_; /// 現在のzoom量

	//// ふるまい
	CameraBehavior behavior_;
	//振る舞いリクエスト
	std::optional<CameraBehavior> behaviorRequest_ = std::nullopt;


	/// other class pointer
	Player* pPlayer_ = nullptr;
	GameCamera* pGameCamera_ = nullptr;
	DirectionalLight* pLight_ = nullptr;


	int prevBehavior_, currentBehavior_;


};