#pragma once

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"

/// ////////////////////////////////////////////
/// primitive mesh creator
/// ////////////////////////////////////////////
class PrimitiveMeshCreator final {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	PrimitiveMeshCreator(AssetCollection* _graphicsResourceCollection, class DxDevice* _dxDevice);
	~PrimitiveMeshCreator();


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	AssetCollection* pGraphicsResourceCollection_ = nullptr;
	class DxDevice* pDxDevice_ = nullptr;

public:


	void CreateCube();
	void CreateSphere();
	void CreateCylinder();
	void CreateCone();
	void CreateTorus();
	void CreatePlane();
};

