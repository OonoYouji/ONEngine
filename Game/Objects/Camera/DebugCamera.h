#pragma once

#include <BaseCamera.h>


/// ===================================================
/// デバッグ用カメラ
/// ===================================================
class DebugCamare final : public BaseCamera {
public:

	DebugCamare();
	~DebugCamare();

	void Initialize() override;
	void Update() override;

};