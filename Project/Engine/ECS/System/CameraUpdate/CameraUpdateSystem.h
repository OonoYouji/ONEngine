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

	void OutsideOfRuntimeUpdate(class EntityComponentSystem* _ecs, const std::vector<class IEntity*>& _entities) override;
	void RuntimeUpdate(class EntityComponentSystem* _ecs, const std::vector<class IEntity*>& _entities) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================
	
	class DxDevice* pDxDevice_;
	class CameraComponent* pMainCamera_;

};

