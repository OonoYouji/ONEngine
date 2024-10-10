#pragma once

#include "Objects/Camera/Manager/CameraManager.h"
#include "Math/Vector3.h"


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

private:

	
};