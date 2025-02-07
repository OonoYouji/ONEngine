#pragma once

/// std
#include <memory>

/// engine
#include "../Interface/IEntity.h"

#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Graphics/Buffer/Data/ViewProjection.h"


/// ===================================================
/// カメラ
/// ===================================================
class Camera : public IEntity {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Camera(class DxDevice* _dxDevice);
	~Camera();

	void Initialize() override;
	void Update()     override;
	void Draw()       override;


private:
	
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unique_ptr<ConstantBuffer<ViewProjection>> viewProjection_;



public:

	/// ===================================================
	/// public : accessors
	/// ===================================================

	/// @brief view projection の取得
	/// @return view projection data
	const ViewProjection& GetViewProjection() const {
		return viewProjection_->GetMappingData(); 
	}

};

