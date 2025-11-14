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


	/// --------------- テクスチャ --------------- ///

	/// @brief テクスチャの読み込みを自動判別で行う(ddsかpngかjpgかなど)
	/// @param _filepath 読み込み対象のファイルパス
	bool LoadTextureAuto(const std::string& _filepath);

	/// @brief テクスチャの新規読み込み
	/// @param _filepath 読み込み対象のファイルパス
	/// @return true: 読み込み成功, false: 読み込み失敗
	bool LoadTexture(const std::string& _filepath);

	/// @brief テクスチャの読み直し
	/// @param _filepath 読み込み対象のファイルパス
	/// @return true: 読み込み成功, false: 読み込み失敗
	bool ReloadTexture(const std::string& _filepath);

	/// @brief Texture3DのDDSファイルの読み込み
	/// @param _filepath 読み込み対象のファイルパス
	/// @return true: 読み込み成功, false: 読み込み失敗
	bool LoadTextureDDS(const std::string& _filepath);
	

	/// --------------- モデル --------------- ///

	/// @brief モデルファイルの読み込み (.obj .gltf)
	/// @param _filepath 読み込み対象のファイルパス
	/// @return true: 読み込み成功, false: 読み込み失敗
	bool LoadModelObj(const std::string& _filepath);

	/// @brief アニメーションのNodeを読み込む
	/// @param _node 読み込み対象のaiNodeポインタ
	/// @return 読み込まれたNode構造体
	Node ReadNode(struct aiNode* _node);

	/// @brief アニメーションの読み込み
	/// @param _model 読み込み対象のModelポインタ
	/// @param _filepath 読み込み対象のファイルパス
	void LoadAnimation(Model* _model, const std::string& _filepath);


	/// --------------- サウンド --------------- ///

	/// @brief オーディオファイルの読み込み
	/// @param _filepath 読み込み対象のファイルパス
	/// @return true: 読み込み成功, false: 読み込み失敗
	bool LoadAudioClip(const std::string& _filepath);


	/// --------------- マテリアル --------------- ///

	/// @brief マテリアルファイルの読み込み
	/// @param _filepath 読み込み対象のファイルパス
	/// @return true: 読み込み成功, false: 読み込み失敗
	bool LoadMaterial(const std::string& _filepath);


	/// --------------- フォント --------------- ///

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

	/// @brief Texture3DのDDSファイルのScratchImageを読み込む
	/// @param _filepath 読み込み対象のファイルパス
	/// @return 生成したScratchImage
	DirectX::ScratchImage LoadScratchImage3D(const std::string& _filepath);

	/// @brief texture resourceを生成する
	/// @param _dxDevice DxDeviceのポインタ
	/// @param _metadata TextureMetadata 構造体への参照
	/// @return texture resource
	[[nodiscard]]
	DxResource CreateTextureResource(class DxDevice* _dxDevice, const DirectX::TexMetadata& _metadata);

	/// @brief Texture3D resourceを生成する
	/// @param _dxDevice DxDeviceのポインタ
	/// @param _metadata TextureMetadata 構造体への参照
	/// @return 
	[[nodiscard]]
	DxResource CreateTexture3DResource(class DxDevice* _dxDevice, const DirectX::TexMetadata& _metadata);

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

	class DxManager*       pDxManager_;
	class AssetCollection* pAssetCollection_;

	unsigned int assimpLoadFlags_; /// assimpで.objを読み込むときに使用するフラグ

};

