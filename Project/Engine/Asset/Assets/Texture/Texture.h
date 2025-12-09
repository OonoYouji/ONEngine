#pragma once

/// std
#include <optional>
#include <string>

/// engine
#include "../Asset.h"
#include "Engine/Core/DirectX12/Resource/DxResource.h"

namespace ONEngine {

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
	Texture(const Vector2& _textureSize);
	~Texture() override = default;


	void CreateEmptySRVHandle();
	void CreateEmptyUAVHandle();


	/// @brief UAVTextureとして作成する
	/// @param _width テクスチャの幅
	/// @param _height テクスチャの高さ
	/// @param _dxDevice DxDeviceへのポインタ
	/// @param _dxSRVHeap DxSRVHeapへのポインタ
	/// @param _dxgiFormat DXGI_FORMAT
	void CreateUAVTexture(UINT _width, UINT _height, class DxDevice* _dxDevice, class DxSRVHeap* _dxSRVHeap, DXGI_FORMAT _dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT);

	/// @brief UAVTexture3Dとして作成する
	/// @param _width テクスチャの幅
	/// @param _height テクスチャの高さ
	/// @param _depth テクスチャの奥行き
	/// @param _dxDevice DxDeviceへのポインタ
	/// @param _dxSRVHeap DxSRVHeapへのポインタ
	/// @param _dxgiFormat DXGI_FORMAT
	void CreateUAVTexture3DWithUAV(UINT _width, UINT _height, UINT _depth, class DxDevice* _dxDevice, class DxSRVHeap* _dxSRVHeap, DXGI_FORMAT _dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT);

	/// @brief 3Dテクスチャに対してUAVの状態を追加する
	/// @param _width テクスチャの幅
	/// @param _height テクスチャの高さ
	/// @param _depth テクスチャの奥行き
	/// @param _dxDevice DxDeviceへのポインタ
	/// @param _dxSRVHeap DxSRVHeapへのポインタ
	/// @param _dxgiFormat DXGI_FORMAT
	void CreateUAVTexture3D(UINT _width, UINT _height, UINT _depth, class DxDevice* _dxDevice, class DxSRVHeap* _dxSRVHeap, DXGI_FORMAT _dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT);


	/// @brief テクスチャをファイルに出力する
	/// @param _filename ファイル名(パス、拡張子込み)
	/// @param _dxDevice DxDeviceへのポインタ
	/// @param _dxCommand DxCommandへのポインタ
	void OutputTexture(const std::wstring& _filename, class DxDevice* _dxDevice, class DxCommand* _dxCommand);
	void OutputTexture3D(const std::wstring& _filename, class DxDevice* _dxDevice, class DxCommand* _dxCommand);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::string name_;

	DxResource dxResource_;
	DxResource readbackTexture_;

	std::optional<Handle> srvHandle_;
	std::optional<Handle> uavHandle_;

	Vector2 textureSize_;
	UINT depth_; // 3Dテクスチャ用

	/// テクスチャのフォーマット、UAVを作成する際に必要
	DXGI_FORMAT srvFormat_;
	DXGI_FORMAT uavFormat_;

public:
	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// ----- setters ----- ///

	void SetName(const std::string& _name);

	/// Handle(cpu, gpu, heap index) を設定
	void SetSRVHandle(const Handle& _handle);
	void SetUAVHandle(const Handle& _handle);

	/// Handle(cpu, gpu, heap index) を設定
	void SetSRVHandle(uint32_t _descriptorIndex, D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE _gpuHandle);
	void SetUAVHandle(uint32_t _descriptorIndex, D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE _gpuHandle);

	/// descriptor index, cpu handle, gpu handle を個別に設定
	void SetSRVDescriptorIndex(uint32_t _index);
	void SetSRVCPUHandle(D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle);
	void SetSRVGPUHandle(D3D12_GPU_DESCRIPTOR_HANDLE _gpuHandle);

	/// descriptor index, cpu handle, gpu handle を個別に設定
	void SetUAVDescriptorIndex(uint32_t _index);
	void SetUAVCPUHandle(D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle);
	void SetUAVGPUHandle(D3D12_GPU_DESCRIPTOR_HANDLE _gpuHandle);


	/// ----- getters ----- ///

	const Handle& GetSRVHandle() const;
	const Handle& GetUAVHandle() const;

	uint32_t GetSRVDescriptorIndex() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUHandle() const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUHandle() const;

	uint32_t GetUAVDescriptorIndex() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetUAVCPUHandle() const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetUAVGPUHandle() const;

	const DxResource& GetDxResource() const;
	DxResource& GetDxResource();

	const Vector2& GetTextureSize() const;
	UINT GetTextureDepth() const;

};


/// @brief TextureをPNG形式で保存する
/// @param _filename ファイル名
/// @param _width テクスチャの幅
/// @param _height テクスチャの高さ
/// @param _overwrite 上書き保存するかどうか
void SaveTextureToPNG(const std::wstring& _filename, size_t _width, size_t _height, bool _overwrite);


void SaveTextureToDDS(const std::wstring& _filename, size_t _width, size_t _height, size_t _depth, bool _overwrite);

} /// ONEngine
