#pragma once

/// directX
#include <d3d12.h>
#include <d3dx12.h>

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

	void CreateCommittedResource(
		class DxDevice* _dxDevice,
		const D3D12_HEAP_PROPERTIES* _pHeapProperties,
		D3D12_HEAP_FLAGS _HeapFlags,
		const D3D12_RESOURCE_DESC* _pDesc,
		D3D12_RESOURCE_STATES _InitialResourceState,
		const D3D12_CLEAR_VALUE* _pOptimizedClearValue
	);

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




/// ===================================================
/// Barrierを作成する関数
/// ===================================================
void CreateBarrier(ID3D12Resource* _resource, D3D12_RESOURCE_STATES _before, D3D12_RESOURCE_STATES _after, class DxCommand* _dxCommand);
