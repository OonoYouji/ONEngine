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

	void Update(class EntityComponentSystem* _ecs, const std::vector<class IEntity*>& _entities) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================
	
	class DxDevice* pDxDevice_;
	class CameraComponent* pMainCamera_;

};

