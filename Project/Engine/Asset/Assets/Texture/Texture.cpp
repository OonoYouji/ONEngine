#include "Texture.h"

/// directX12
#include <wrl/client.h>
#include <wincodec.h>

/// std
#include <cassert>
#include <iostream>

/// externals
#include <DirectXTex.h>
#include <magic_enum/magic_enum.hpp>

/// engine
#include "Engine/Core/Utility/Tools/Assert.h"
#include "Engine/Core/DirectX12/Device/DxDevice.h"
#include "Engine/Core/DirectX12/DescriptorHeap/DxSRVHeap.h"
#include "Engine/Core/DirectX12/Command/DxCommand.h"

using namespace ONEngine;

namespace {
	/// printf 互換のフォーマットログ
	template <class... Args>
	void Printf(const char* _fmt, Args... _args) {
		// 出力サイズ計算
		int size = std::snprintf(nullptr, 0, _fmt, _args...);
		if (size <= 0) {
			Console::Log("Format error");
			return;
		}

		// サイズ分の文字列を生成
		std::string msg(size, '\0');

		// 実際にフォーマット文字列を埋め込む
		std::snprintf(&msg[0], size + 1, _fmt, _args...);

		// Console::Log に渡す
		Console::Log(msg);
	}
}


Texture::Texture() = default;

Texture::Texture(const Vector2& _textureSize)
	: textureSize_(_textureSize) {
}

void Texture::CreateEmptySRVHandle() {
	srvHandle_.emplace(Handle());
}

void Texture::CreateEmptyUAVHandle() {
	uavHandle_.emplace(Handle());
}

void Texture::CreateUAVTexture(UINT _width, UINT _height, DxDevice* _dxDevice, DxSRVHeap* _dxSRVHeap, DXGI_FORMAT _dxgiFormat) {
	// テクスチャディスクリプション
	D3D12_RESOURCE_DESC texDesc = {};
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = _width;
	texDesc.Height = _height;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = _dxgiFormat;
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
	uavDesc.Format = _dxgiFormat;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;
	uavDesc.Texture2D.PlaneSlice = 0;

	uint32_t index = _dxSRVHeap->AllocateUAVTexture();
	CreateEmptyUAVHandle();
	SetUAVDescriptorIndex(index);
	SetUAVCPUHandle(_dxSRVHeap->GetCPUDescriptorHandel(index));
	SetUAVGPUHandle(_dxSRVHeap->GetGPUDescriptorHandel(index));

	_dxDevice->GetDevice()->CreateUnorderedAccessView(dxResource_.Get(), nullptr, &uavDesc, uavHandle_->cpuHandle);



	/// ログに今回行った操作を出力
	Console::Log("[Create UAV Texture] ");
	Console::Log(" - Width: " + std::to_string(_width));
	Console::Log(" - Height: " + std::to_string(_height));
	Console::Log(" - Format: " + std::string(magic_enum::enum_name(_dxgiFormat)));
	Console::Log(" - DescriptorIndex: " + std::to_string(uavHandle_->descriptorIndex));
	Console::Log(" - Dimension: Texture2D");

}


void Texture::CreateUAVTexture3DWithUAV(
	UINT _width, UINT _height, UINT _depth,
	DxDevice* _dxDevice,
	DxSRVHeap* _dxSRVHeap,
	DXGI_FORMAT _uavFormat) {

	ID3D12Device* device = _dxDevice->GetDevice();
	uavFormat_ = _uavFormat;

	/// --------------- UAV Descriptor 設定 --------------- ///
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = srvFormat_;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
	uavDesc.Texture3D.MipSlice = 0;
	uavDesc.Texture3D.FirstWSlice = 0;
	uavDesc.Texture3D.WSize = _depth;

	uint32_t descriptorIndex;
	if (!uavHandle_.has_value()) {
		/// 新規作成
		CreateEmptyUAVHandle();
		descriptorIndex = _dxSRVHeap->AllocateUAVTexture();
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = _dxSRVHeap->GetCPUDescriptorHandel(descriptorIndex);
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = _dxSRVHeap->GetGPUDescriptorHandel(descriptorIndex);
		SetUAVHandle(descriptorIndex, cpuHandle, gpuHandle);

		Assert(descriptorIndex != 1029 + 2048);
	} else {
		/// 既存のハンドルを使用
		descriptorIndex = uavHandle_->descriptorIndex;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = _dxSRVHeap->GetCPUDescriptorHandel(descriptorIndex);

	/// --------------- UAV生成 --------------- ///
	device->CreateUnorderedAccessView(
		dxResource_.Get(),
		nullptr,
		&uavDesc,
		cpuHandle
	);

	dxResource_.SetCurrentState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	/// --------------- ログ --------------- ///
	Console::Log("[Create UAV Texture3D]");
	Console::Log(" - Texture Name: " + name_);
	Console::Log(" - Width: " + std::to_string(_width));
	Console::Log(" - Height: " + std::to_string(_height));
	Console::Log(" - Depth: " + std::to_string(_depth));
	Console::Log(" - UAV Format: " + std::string(magic_enum::enum_name(uavFormat_)));
	Console::Log(" - SRV Format: " + std::string(magic_enum::enum_name(srvFormat_)));
	Console::Log(" - DescriptorIndex: " + std::to_string(descriptorIndex));
}


void Texture::CreateUAVTexture3D(UINT _width, UINT _height, UINT _depth, DxDevice* _dxDevice, DxSRVHeap* _dxSRVHeap, DXGI_FORMAT _dxgiFormat) {
	// テクスチャディスクリプション
	D3D12_RESOURCE_DESC texDesc = {};
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
	texDesc.Alignment = 0;
	texDesc.Width = _width;
	texDesc.Height = _height;
	texDesc.DepthOrArraySize = _depth;
	texDesc.MipLevels = 1;
	texDesc.Format = _dxgiFormat;
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
	uavDesc.Format = _dxgiFormat;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
	uavDesc.Texture3D.MipSlice = 0;
	uavDesc.Texture3D.FirstWSlice = 0;
	uavDesc.Texture3D.WSize = _depth;

	uint32_t index = _dxSRVHeap->AllocateUAVTexture();
	CreateEmptyUAVHandle();
	SetUAVDescriptorIndex(index);
	SetUAVCPUHandle(_dxSRVHeap->GetCPUDescriptorHandel(index));
	SetUAVGPUHandle(_dxSRVHeap->GetGPUDescriptorHandel(index));

	_dxDevice->GetDevice()->CreateUnorderedAccessView(dxResource_.Get(), nullptr, &uavDesc, uavHandle_->cpuHandle);

	/// ログに今回行った操作を出力
	Console::Log("[Create UAV Texture3D]");
	Console::Log(" - Texture Name: " + name_);
	Console::Log(" - Width: " + std::to_string(_width));
	Console::Log(" - Height: " + std::to_string(_height));
	Console::Log(" - Depth: " + std::to_string(_depth));
	Console::Log(" - UAV Format: " + std::string(magic_enum::enum_name(uavFormat_)));
	Console::Log(" - SRV Format: " + std::string(magic_enum::enum_name(srvFormat_)));
	Console::Log(" - DescriptorIndex: " + std::to_string(index));
}

void Texture::OutputTexture(const std::wstring& _filename, DxDevice* _dxDevice, DxCommand* _dxCommand) {
	/// Readbackリソースを作成（1行ごとのAlignmentに注意）
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

	/// dst (Readback側) の設定
	D3D12_TEXTURE_COPY_LOCATION dst = {};
	dst.pResource = readbackTexture_.Get();
	dst.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	dst.PlacedFootprint = footprint;

	/// src (UAV側) の設定
	D3D12_TEXTURE_COPY_LOCATION src = {};
	src.pResource = dxResource_.Get();
	src.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	src.SubresourceIndex = 0;

	dxResource_.CreateBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, _dxCommand);
	_dxCommand->GetCommandList()->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
	dxResource_.CreateBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, _dxCommand);

	/// コマンドの実行&リセット
	_dxCommand->CommandExecuteAndWait();
	_dxCommand->CommandReset();


	/// ReadbackしてDirectXTexに詰め替え
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

	/// .pngで保存
	DirectX::SaveToWICFile(
		*scratch.GetImage(0, 0, 0),
		DirectX::WIC_FLAGS_NONE,
		GUID_ContainerFormatPng,
		_filename.c_str()
	);

	readbackTexture_.Get()->Unmap(0, nullptr);
}

void Texture::OutputTexture3D(const std::wstring& _filename, DxDevice* _dxDevice, DxCommand* _dxCommand) {
	auto desc = dxResource_.Get()->GetDesc();
	if (desc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE3D) {
		Assert(false, "Not a 3D texture.");
	}

	// Readback用バッファサイズを取得
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint{};
	UINT numRows = 0;
	UINT64 rowPitch = 0;
	UINT64 totalBytes = 0;
	_dxDevice->GetDevice()->GetCopyableFootprints(
		&desc,
		0,      // FirstSubresource
		1,      // NumSubresources = 1 for 3D texture
		0,
		&footprint,
		&numRows,
		&rowPitch,
		&totalBytes
	);

	// Readbackバッファを作成
	CD3DX12_RESOURCE_DESC rbDesc = CD3DX12_RESOURCE_DESC::Buffer(totalBytes);
	D3D12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK);

	DxResource readback;
	readback.CreateCommittedResource(
		_dxDevice,
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&rbDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr
	);

	// コピー前にリソース状態を変更
	dxResource_.CreateBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, _dxCommand);

	D3D12_TEXTURE_COPY_LOCATION src = {};
	src.pResource = dxResource_.Get();
	src.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	src.SubresourceIndex = 0;

	D3D12_TEXTURE_COPY_LOCATION dst = {};
	dst.pResource = readback.Get();
	dst.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	dst.PlacedFootprint = footprint;

	// コピー
	_dxCommand->GetCommandList()->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

	// コピー後、UAVに戻す場合
	dxResource_.CreateBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, _dxCommand);

	_dxCommand->CommandExecuteAndWait();
	_dxCommand->CommandReset();

	// ReadbackしてDirectXTexに詰め替え
	D3D12_RANGE range{ 0, static_cast<SIZE_T>(totalBytes) };
	void* mapped = nullptr;
	readback.Get()->Map(0, &range, &mapped);

	DirectX::ScratchImage volumeScratch;
	volumeScratch.Initialize3D(desc.Format, desc.Width, desc.Height, desc.DepthOrArraySize, desc.MipLevels);

	for (UINT z = 0; z < desc.DepthOrArraySize; z++) {
		size_t sliceOffset = footprint.Offset + z * footprint.Footprint.RowPitch * footprint.Footprint.Height;

		DirectX::Image img = {};
		img.format = desc.Format;
		img.width = static_cast<size_t>(desc.Width);
		img.height = static_cast<size_t>(desc.Height);
		img.rowPitch = footprint.Footprint.RowPitch;
		img.slicePitch = footprint.Footprint.RowPitch * desc.Height;
		img.pixels = reinterpret_cast<uint8_t*>(mapped) + sliceOffset;

		memcpy(volumeScratch.GetImage(0, 0, z)->pixels, img.pixels, img.slicePitch);
	}

	DirectX::SaveToDDSFile(
		volumeScratch.GetImages(),
		volumeScratch.GetImageCount(),
		volumeScratch.GetMetadata(),
		DirectX::DDS_FLAGS_NONE,
		_filename.c_str()
	);

	readback.Get()->Unmap(0, nullptr);
}



void Texture::SetName(const std::string& _name) {
	name_ = _name;
	if (dxResource_.Get()) {
		dxResource_.Get()->SetName(std::wstring(_name.begin(), _name.end()).c_str());
	}
}


void Texture::SetSRVHandle(const Handle& _handle) {
	srvHandle_ = _handle;
}

void Texture::SetUAVHandle(const Handle& _handle) {
	uavHandle_ = _handle;
}

void Texture::SetSRVHandle(uint32_t _descriptorIndex, D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE _gpuHandle) {
	srvHandle_ = Handle{ _descriptorIndex, _cpuHandle, _gpuHandle };
}

void Texture::SetUAVHandle(uint32_t _descriptorIndex, D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE _gpuHandle) {
	uavHandle_ = Handle{ _descriptorIndex, _cpuHandle, _gpuHandle };
}

void Texture::SetSRVDescriptorIndex(uint32_t _index) {
	srvHandle_->descriptorIndex = _index;
}

void Texture::SetSRVCPUHandle(D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle) {
	srvHandle_->cpuHandle = _cpuHandle;
}

void Texture::SetSRVGPUHandle(D3D12_GPU_DESCRIPTOR_HANDLE _gpuHandle) {
	srvHandle_->gpuHandle = _gpuHandle;
}

void Texture::SetUAVDescriptorIndex(uint32_t _index) {
	uavHandle_->descriptorIndex = _index;
}

void Texture::SetUAVCPUHandle(D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle) {
	uavHandle_->cpuHandle = _cpuHandle;
}

void Texture::SetUAVGPUHandle(D3D12_GPU_DESCRIPTOR_HANDLE _gpuHandle) {
	uavHandle_->gpuHandle = _gpuHandle;
}

const Texture::Handle& Texture::GetSRVHandle() const {
	Assert(srvHandle_.has_value());
	return srvHandle_.value();
}

const Texture::Handle& Texture::GetUAVHandle() const {
	Assert(uavHandle_.has_value());
	return uavHandle_.value();
}

uint32_t Texture::GetSRVDescriptorIndex() const {
	Assert(srvHandle_.has_value());
	return srvHandle_->descriptorIndex;
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetSRVCPUHandle() const {
	Assert(srvHandle_.has_value());
	return srvHandle_->cpuHandle;
}

D3D12_GPU_DESCRIPTOR_HANDLE Texture::GetSRVGPUHandle() const {
	Assert(srvHandle_.has_value());
	return srvHandle_->gpuHandle;
}

uint32_t Texture::GetUAVDescriptorIndex() const {
	Assert(uavHandle_.has_value());
	return uavHandle_->descriptorIndex;
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetUAVCPUHandle() const {
	Assert(uavHandle_.has_value());
	return uavHandle_->cpuHandle;
}

D3D12_GPU_DESCRIPTOR_HANDLE Texture::GetUAVGPUHandle() const {
	Assert(uavHandle_.has_value());
	return uavHandle_->gpuHandle;
}

const DxResource& Texture::GetDxResource() const {
	return dxResource_;
}

DxResource& Texture::GetDxResource() {
	return dxResource_;
}

const Vector2& Texture::GetTextureSize() const {
	return textureSize_;
}

UINT Texture::GetTextureDepth() const {
	return depth_;
}



void ONEngine::SaveTextureToPNG(const std::wstring& _filename, size_t _width, size_t _height, bool _overwrite) {

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

void ONEngine::SaveTextureToDDS(const std::wstring& _filename, size_t _width, size_t _height, size_t _depth, bool _overwrite) {

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


	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	size_t rowPitch = _width * 4;

	// 3Dテクスチャ用 ScratchImage
	DirectX::ScratchImage volumeImage;
	HRESULT hr = volumeImage.Initialize3D(format, _width, _height, _depth, 1);
	Assert(SUCCEEDED(hr));

	// 仮のデータで埋める（上半分を透明にする）
	for (size_t z = 0; z < _depth; ++z) {

		// 0.0 ～ 1.0 のグラデーション係数
		float t = static_cast<float>(z) / static_cast<float>(_depth - 1);

		const DirectX::Image* img = volumeImage.GetImage(0, 0, z);
		uint8_t* dst = img->pixels;

		for (size_t y = 0; y < _height; ++y) {
			for (size_t x = 0; x < _width; ++x) {

				size_t index = y * rowPitch + x * 4;

				// t に応じたグラデーション
				uint8_t r = static_cast<uint8_t>(255 * t);        // 黒 → 赤
				uint8_t g = static_cast<uint8_t>(128 * (1 - t));  // 緑成分が減る例
				uint8_t b = static_cast<uint8_t>(255 * (1 - t));  // 青 → 黒

				// 上半分を透明にする
				uint8_t a = (y < _height / 2) ? 0 : 255;
				if (a == 0) {
					r = 0;
					g = 0;
					b = 0;
				}

				dst[index + 0] = r;
				dst[index + 1] = g;
				dst[index + 2] = b;
				dst[index + 3] = a;
			}
		}
	}


	// 仮のデータで埋める（ピラミッド型のグラデーション）
	//for (size_t z = 0; z < _depth; ++z) {
	//	const DirectX::Image* img = volumeImage.GetImage(0, 0, z);
	//	uint8_t* dst = img->pixels;

	//	for (size_t y = 0; y < _height; ++y) {
	//		for (size_t x = 0; x < _width; ++x) {
	//			size_t index = y * rowPitch + x * 4;

	//			// 上の数ピクセルを空白にする
	//			if (y < 5) { // 例えば上5ピクセルを空白にする
	//				dst[index + 0] = 0; // R
	//				dst[index + 1] = 0; // G
	//				dst[index + 2] = 0; // B
	//				dst[index + 3] = 0; // A
	//				continue;
	//			}

	//			// ピクセルの透明度を計算
	//			uint8_t alpha = (x + y >= _width - 1) ? 255 : 0;

	//			// ピクセルデータを設定
	//			dst[index + 0] = 0;      // R
	//			dst[index + 1] = 0;      // G
	//			dst[index + 2] = 0;      // B
	//			dst[index + 3] = alpha;  // A
	//		}
	//	}
	//}


	// DDS 保存
	hr = DirectX::SaveToDDSFile(
		volumeImage.GetImages(),
		volumeImage.GetImageCount(),
		volumeImage.GetMetadata(),
		DirectX::DDS_FLAGS_NONE,
		_filename.c_str()
	);

	Assert(SUCCEEDED(hr));

}

