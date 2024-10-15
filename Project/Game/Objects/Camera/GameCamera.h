#pragma once

#include "Objects/Camera/Manager/CameraManager.h"
#include"Math/Vector2.h"
//std
#include<optional>

/// ===================================================
/// ゲームシーン用カメラ
/// ===================================================
class GameCamera final : public BaseCamera {
public:

	GameCamera(const std::string& name);
	~GameCamera();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	//振る舞い管理
	void BehaviorManagement();
	//振る舞い関数
	void RootInit();
	void RootUpdate();
	void ZoomInInit();
	void ZoomInUpdate();
	void ZoomOutInit();
	void ZoomOutUpdate();
	//getter

	//setter
	void SetBehaviorZoomOut();//ズームアウト
	void SetBehaviorZoomIn();//ズームイン
private:
	enum class Behavior {
		kRoot,
		kZoomIn,
		kZoomOut,
	};
private:
	float easeT_;
	float easeTMax_;
	float zoomOutMax_;
	float zoomInMax_;
	//// ふるまい
	Behavior behavior_;
	//振る舞いリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	
};