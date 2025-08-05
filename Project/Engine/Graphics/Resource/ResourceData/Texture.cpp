#include "Texture.h"

/// directX12
#include <wrl/client.h>
#include <wincodec.h>

/// std
#include <cassert>
#include <iostream>

/// externals
#include <DirectXTex.h>

/// engine
#include "Engine/Core/Utility/Tools/Assert.h"
#include "Engine/Core/DirectX12/Device/DxDevice.h"
#include "Engine/Core/DirectX12/DescriptorHeap/DxSRVHeap.h"
#include "Engine/Core/DirectX12/Command/DxCommand.h"


Texture::Texture() {}
Texture::~Texture() {}

void Texture::CreateEmptySRVHandle() {
	srvHandle_.emplace(Handle());
}

void Texture::CreateEmptyUAVHandle() {
	uavHandle_.emplace(Handle());
}

void Texture::CreateUAVTexture(UINT _width, UINT _height, DxDevice* _dxDevice, DxSRVHeap* _dxSRVHeap) {
	// テクスチャディスクリプション
	D3D12_RESOURCE_DESC texDesc = {};
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = _width;
	texDesc.Height = _height;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	// リソース作成
	D3D12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	dxResource_.CreateCommittedResource(
		_dxDevice, &heapProperties, D3D12_HEAP_FLAG_NONE,
		&texDesc, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr
	);

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;
	uavDesc.Texture2D.PlaneSlice = 0;

	uint32_t index = _dxSRVHeap->AllocateBuffer();
	CreateEmptyUAVHandle();
	SetUAVDescriptorIndex(index);
	SetUAVCPUHandle(_dxSRVHeap->GetCPUDescriptorHandel(index));
	SetUAVGPUHandle(_dxSRVHeap->GetGPUDescriptorHandel(index));

	_dxDevice->GetDevice()->CreateUnorderedAccessView(dxResource_.Get(), nullptr, &uavDesc, uavHandle_->cpuHandle);

}

void Texture::OutputTexture(const std::wstring& _filename, DxDevice* _dxDevice, DxCommand* _dxCommand) {
	// 1. Readbackリソースを作成（1行ごとのAlignmentに注意）
	D3D12_RESOURCE_DESC desc = dxResource_.Get()->GetDesc();

	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = {};
	UINT numRows = 0;
	UINT64 rowPitch = 0;
	UINT64 totalBytes = 0;
	_dxDevice->GetDevice()->GetCopyableFootprints(&desc, 0, 1, 0, &footprint, &numRows, &rowPitch, &totalBytes);

	CD3DX12_RESOURCE_DESC rbDesc = CD3DX12_RESOURCE_DESC::Buffer(totalBytes);
	D3D12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK);
	readbackTexture_.CreateCommittedResource(
		_dxDevice, &heapProperties, D3D12_HEAP_FLAG_NONE,
		&rbDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr
	);

	// dst (Readback側) の設定
	D3D12_TEXTURE_COPY_LOCATION dst = {};
	dst.pResource = readbackTexture_.Get();
	dst.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	dst.PlacedFootprint = footprint;

	// src (UAV側) の設定
	D3D12_TEXTURE_COPY_LOCATION src = {};
	src.pResource = dxResource_.Get();
	src.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	src.SubresourceIndex = 0;

	dxResource_.CreateBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, _dxCommand);
	_dxCommand->GetCommandList()->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
	dxResource_.CreateBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, _dxCommand);

	/// コマンドの実行&リセット
	_dxCommand->CommandExecute();
	_dxCommand->CommandReset();


	// 4. ReadbackしてDirectXTexに詰め替え
	D3D12_RANGE range = { 0, static_cast<SIZE_T>(totalBytes) };
	void* mappedData = nullptr;
	readbackTexture_.Get()->Map(0, &range, &mappedData);

	DirectX::Image image = {};
	image.width = static_cast<size_t>(desc.Width);
	image.height = desc.Height;
	image.format = desc.Format;
	image.rowPitch = static_cast<size_t>(footprint.Footprint.RowPitch);
	image.slicePitch = static_cast<size_t>(footprint.Footprint.RowPitch) * desc.Height;
	image.pixels = reinterpret_cast<uint8_t*>(mappedData);

	DirectX::ScratchImage scratch;
	scratch.InitializeFromImage(image);

	// 5. 保存（GUID_ContainerFormatPngはwincodec.hに定義あり）
	DirectX::SaveToWICFile(*scratch.GetImage(0, 0, 0),
		DirectX::WIC_FLAGS_NONE,
		GUID_ContainerFormatPng,
		_filename.c_str());

	readbackTexture_.Get()->Unmap(0, nullptr);
}

void Texture::SetName(const std::string& _name) {
	name_ = _name;
	if (dxResource_.Get()) {
		dxResource_.Get()->SetName(std::wstring(_name.begin(), _name.end()).c_str());
	}
}

void SaveTextureToPNG(const std::wstring& _filename, size_t _width, size_t _height, bool _overwrite) {

	/// _filenameの先のディレクトリが存在しない場合は作成
	std::filesystem::path filePath(_filename);
	if (!std::filesystem::exists(filePath.parent_path())) {
		std::filesystem::create_directories(filePath.parent_path());
	} else {
		// ファイルが既に存在しているかチェック
		if (std::filesystem::is_regular_file(filePath)) {
			/// あった場合上書きするのかどうか
			if (!_overwrite) {
				return;
			}
		}
	}


	// テクスチャのサイズとフォーマット
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	const size_t rowPitch = _width * 4;       // 1ピクセル4バイト（RGBA）
	const size_t slicePitch = rowPitch * _height;

	std::vector<uint8_t> pixelData(slicePitch);
	for (size_t y = 0; y < _height; ++y) {
		for (size_t x = 0; x < _width; ++x) {
			size_t index = y * rowPitch + x * 4;
			pixelData[index + 0] = 255; // R
			pixelData[index + 1] = 255; // G
			pixelData[index + 2] = 255; // B
			pixelData[index + 3] = 255; // A
		}
	}

	// ScratchImage を作成
	DirectX::ScratchImage image;
	HRESULT hr = image.Initialize2D(format, _width, _height, 1, 1);
	Assert(SUCCEEDED(hr));

	// ピクセルデータをコピー
	const DirectX::Image* img = image.GetImage(0, 0, 0);
	std::memcpy(img->pixels, pixelData.data(), slicePitch);

	// PNG 形式で保存
	hr = DirectX::SaveToWICFile(*img, DirectX::WIC_FLAGS_NONE, GUID_ContainerFormatPng, _filename.c_str());
	Assert(SUCCEEDED(hr));

}

