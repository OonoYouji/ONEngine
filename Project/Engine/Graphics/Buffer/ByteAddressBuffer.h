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

	/// @brief Bufferを作成する
	/// @param _size Bufferのサイズ
	/// @param _dxDevice DxDeviceのポインタ
	/// @param _dxSRVHeap DxSRVHeapのポインタ
	void Create(uint32_t _size, DxDevice* _dxDevice, DxSRVHeap* _dxSRVHeap);


	/// @brief 指定したインデックスに対応するマップされたデータの値を設定する
	/// @param _index 設定対象のデータのインデックス
	/// @param _value 設定する値（32ビット符号なし整数）
	void SetMappedData(size_t _index, uint32_t _value);

	/// @brief コマンドリストにバインドする
	/// @param _rootParameterIndex パラメータインデックス
	/// @param _commandList CommandListのポインタ
	void BindToCommandList(UINT _rootParameterIndex, ID3D12GraphicsCommandList* _commandList);

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

	size_t                      totalSize_;
	size_t                      bufferSize_;

	DxSRVHeap* pDxSRVHeap_;


};