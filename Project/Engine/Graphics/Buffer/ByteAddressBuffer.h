#pragma once

/// std
#include <vector>
#include <span>

/// engine
#include "Engine/Core/DirectX12/ComPtr/ComPtr.h"
#include "Engine/Core/DirectX12/Resource/DxResource.h"
#include "Engine/Core/DirectX12/Device/DxDevice.h"
#include "Engine/Core/DirectX12/DescriptorHeap/DxSRVHeap.h"

/// //////////////////////////////////////////////////////
/// ByteAddressBuffer
/// //////////////////////////////////////////////////////
class ByteAddressBuffer final {
public:
	/// ==================================================
	/// public : methods
	/// ==================================================

	ByteAddressBuffer();
	~ByteAddressBuffer();

	void Create(uint32_t _size, DxDevice* _dxDevice, DxSRVHeap* _dxSRVHeap);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	uint32_t                    srvDescriptorIndex_;
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle_;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle_;

	DxResource                  bufferResource_;
	uint32_t*                   mappedData_;
	std::span<uint32_t>         mappedDataArray_;

	//size_t                      structureSize_;
	size_t                      totalSize_;
	size_t                      bufferSize_;

	DxSRVHeap* pDxSRVHeap_;

public:

	void SetMappedData(size_t _index, uint32_t _value);

	void BindToCommandList(UINT _rootParameterIndex, ID3D12GraphicsCommandList* _commandList);

};