#pragma once

/// engine
#include "Engine/Core/DirectX12/Resource/DxResource.h"


/// ===================================================
/// texture
/// ===================================================
class Texture final {
	friend class GraphicsResourceLoader;
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Texture();
	~Texture();

public:
	
	/// ===================================================
	/// public : objects
	/// ===================================================

	DxResource dxResource_;

};

