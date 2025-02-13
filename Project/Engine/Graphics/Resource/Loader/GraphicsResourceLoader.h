#pragma once

/// directX
#include <d3d12.h>

/// std
#include <string>
#include <vector>
#include <memory>

/// externals
#include <DirectXTex.h>

/// engine
#include "Engine/Core/DirectX12/ComPtr/ComPtr.h"
#include "Engine/Core/DirectX12/Resource/DxResource.h"


/// ===================================================
/// グラフィクスリソースのローダー
/// ===================================================
class GraphicsResourceLoader final {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	GraphicsResourceLoader(class DxManager* _dxManager, class GraphicsResourceCollection* _collection);
	~GraphicsResourceLoader();

	void Initialize();

	/// @brief textureの読み込み (.png, .jpg)
	/// @param _filePath .slnファイルからの相対パス
	void LoadTexture(const std::string& _filePath);

	/// @brief modelの読み込み (.obj)
	/// @param _filePath .slnファイルからの相対パス
	void LoadModelObj(const std::string& _filePath);

private:

	/// ===================================================
	/// private : methods
	/// ===================================================

	/// @brief scratchImageを読み込む
	/// @param _filePath .slnファイルからの相対パス
	/// @return DirectX::ScratchImage
	DirectX::ScratchImage LoadScratchImage(const std::string& _filePath);

	/// @brief texture resourceを生成する
	/// @param _dxDevice DxDeviceのポインタ
	/// @param _metadata TextureMetadata 構造体への参照
	/// @return texture resource
	DxResource CreateTextureResource(class DxDevice* _dxDevice, const DirectX::TexMetadata& _metadata);

	/// @brief texture dataをアップロードする
	/// @param _texture texture resource
	/// @param _mipImages scratchImage
	/// @return 
	[[nodiscard]]
	DxResource UploadTextureData(ID3D12Resource* _texture, const DirectX::ScratchImage& _mipScratchImage);


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class DxManager*                  dxManager_;
	class GraphicsResourceCollection* resourceCollection_;

	unsigned int assimpLoadFlags_; /// assimpで.objを読み込むときに使用するフラグ


};

