#pragma once

/// directX
#include <d3d12.h>

/// engine
#include "../ComPtr/ComPtr.h"


/// ===================================================
/// ID3D12Resourceのラッパークラス
/// ===================================================
class DxResource final {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	DxResource();
	~DxResource();

	/// @brief bufferを作成する
	/// @param _dxDevice 
	/// @param _sizeInByte 
	void CreateResource(class DxDevice* _dxDevice, size_t _sizeInByte);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	ComPtr<ID3D12Resource> resource_ = nullptr;


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	ID3D12Resource* Get() const { return resource_.Get(); }

};

