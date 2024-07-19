#pragma once

#include <CameraManager.h>


/// ===================================================
/// ゲームシーン用カメラ
/// ===================================================
class GameCamera final : public BaseCamera {
public:

	GameCamera();
	~GameCamera();

	void Initialize() override;
	void Update() override;

private:



};