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


Texture::Texture() {}
Texture::~Texture() {}

void Texture::CreateEmptySRVHandle() {
	srvHandle_.emplace(Handle());
}

void Texture::CreateEmptyUAVHandle() {
	uavHandle_.emplace(Handle());
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

