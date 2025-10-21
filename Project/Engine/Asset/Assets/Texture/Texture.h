#pragma once

/// std
#include <optional>
#include <string>

/// engine
#include "../Asset.h"
#include "Engine/Core/DirectX12/Resource/DxResource.h"

/// ///////////////////////////////////////////////////
/// texture
/// ///////////////////////////////////////////////////
class Texture final : public IAsset {
	friend class AssetLoader;
public:
	/// ===================================================
	/// public : sub class
	/// ===================================================

	struct Handle {
		uint32_t descriptorIndex;
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	Texture();
	~Texture() override = default;


	void CreateEmptySRVHandle();
	void CreateEmptyUAVHandle();

	void CreateUAVTexture(UINT _width, UINT _height, class DxDevice* _dxDevice, class DxSRVHeap* _dxSRVHeap, DXGI_FORMAT _dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT);

	void OutputTexture(const std::wstring& _filename, class DxDevice* _dxDevice, class DxCommand* _dxCommand);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::string name_;

	DxResource                  dxResource_;
	DxResource readbackTexture_;

	std::optional<Handle>       srvHandle_;
	std::optional<Handle>       uavHandle_;

public:
	/// ===================================================
	/// public : accessor
	/// ===================================================

	void SetName(const std::string& _name);

	/// @brief SRV handleを設定
	/// @param _handle cpu, gpu handle
	void SetSRVHandle(const Handle& _handle) { srvHandle_ = _handle; }

	/// @brief UAV handleを設定
	/// @param _handle cpu, gpu handle
	void SetUAVHandle(const Handle& _handle) { uavHandle_ = _handle; }

	/// @brief SRV handleを設定
	/// @param _descriptorIndex descriptor index
	/// @param _cpuHandle cpu handle
	/// @param _gpuHandle gpu handle
	void SetSRVHandle(uint32_t _descriptorIndex, D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE _gpuHandle) {
		srvHandle_ = Handle{ _descriptorIndex, _cpuHandle, _gpuHandle };
	}

	/// @brief UAV handleを設定
	/// @param _descriptorIndex descriptor index
	/// @param _cpuHandle cpu handle
	/// @param _gpuHandle gpu handle
	void SetUAVHandle(uint32_t _descriptorIndex, D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE _gpuHandle) {
		uavHandle_ = Handle{ _descriptorIndex, _cpuHandle, _gpuHandle };
	}

	void SetSRVDescriptorIndex(uint32_t _index) { srvHandle_->descriptorIndex = _index; }
	void SetSRVCPUHandle(D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle) { srvHandle_->cpuHandle = _cpuHandle; }
	void SetSRVGPUHandle(D3D12_GPU_DESCRIPTOR_HANDLE _gpuHandle) { srvHandle_->gpuHandle = _gpuHandle; }

	void SetUAVDescriptorIndex(uint32_t _index) { uavHandle_->descriptorIndex = _index; }
	void SetUAVCPUHandle(D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle) { uavHandle_->cpuHandle = _cpuHandle; }
	void SetUAVGPUHandle(D3D12_GPU_DESCRIPTOR_HANDLE _gpuHandle) { uavHandle_->gpuHandle = _gpuHandle; }



	const Handle& GetSRVHandle() const { return *srvHandle_; }
	const Handle& GetUAVHandle() const { return *uavHandle_; }

	uint32_t GetSRVDescriptorIndex() const { return srvHandle_->descriptorIndex; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUHandle() const { return srvHandle_->cpuHandle; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUHandle() const { return srvHandle_->gpuHandle; }

	uint32_t GetUAVDescriptorIndex() const { return uavHandle_->descriptorIndex; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetUAVCPUHandle() const { return uavHandle_->cpuHandle; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetUAVGPUHandle() const { return uavHandle_->gpuHandle; }

	/// @brief resourceを取得
	/// @return DxResource
	const DxResource& GetDxResource() const { return dxResource_; }

	/// @brief resourceを取得
	/// @return DxResource
	DxResource& GetDxResource() { return dxResource_; }
};


void SaveTextureToPNG(const std::wstring& _filename, size_t _width, size_t _height, bool _overwrite);