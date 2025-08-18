#pragma once

/// engine
#include "../Interface/ECSISystem.h"

/// ///////////////////////////////////////////////////
/// カメラ更新システム
/// ///////////////////////////////////////////////////
class CameraUpdateSystem : public ECSISystem {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	CameraUpdateSystem(class DxDevice* _dxDevice);
	~CameraUpdateSystem() override = default;

	void OutsideOfRuntimeUpdate(class ECSGroup* _ecs) override;
	void RuntimeUpdate(class ECSGroup* _ecs) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================
	
	class DxDevice* pDxDevice_;
	class CameraComponent* pMainCamera_;

};

