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
#include "Engine/Asset/Assets/Mesh/Skinning.h"


/// ///////////////////////////////////////////////////
/// Assetのローダー
/// ///////////////////////////////////////////////////
class AssetLoader final {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	AssetLoader(class DxManager* _dxm, class AssetCollection* _collection);
	~AssetLoader();

	void Initialize();

	/// テクスチャ
	void LoadTexture(const std::string& _filepath);

	/// モデル
	void LoadModelObj(const std::string& _filepath);
	Node ReadNode(struct aiNode* _node);
	void LoadAnimation(Model* _model, const std::string& _filepath);

	/// サウンド
	void LoadAudioClip(const std::string& _filepath);

	/// マテリアル
	void LoadMaterial(const std::string& _filepath);

	/// フォント
	void LoadFont(const std::string& _filepath);

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	/// @brief scratchImageを読み込む
	/// @param _filePath .slnファイルからの相対パス
	/// @return DirectX::ScratchImage
	DirectX::ScratchImage LoadScratchImage(const std::string& _filepath);

	/// @brief texture resourceを生成する
	/// @param _dxDevice DxDeviceのポインタ
	/// @param _metadata TextureMetadata 構造体への参照
	/// @return texture resource
	[[nodiscard]]
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

	class DxManager*                  pDxManager_;
	class AssetCollection* pAssetCollection_;

	unsigned int assimpLoadFlags_; /// assimpで.objを読み込むときに使用するフラグ


};

