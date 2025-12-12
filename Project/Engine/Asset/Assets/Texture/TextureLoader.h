#pragma once

/// directX
#include <d3d12.h>
#include <DirectXTex.h>

/// engine
#include "../IAssetLoader.h"
#include "Texture.h"

#include "Engine/Core/DirectX12/Resource/DxResource.h"


namespace ONEngine {

/// ///////////////////////////////////////////////////
/// Texture用のアセットローダー
/// ///////////////////////////////////////////////////
template<>
class AssetLoaderT<Texture> : public IAssetLoader {
public:

	AssetLoaderT(DxManager* _dxm, AssetCollection* _ac);
	~AssetLoaderT() override = default;

	/// @brief テクスチャの読み込みを行う
	/// @param _filepath 対象のファイルパス
	/// @return 読み込んだテクスチャ
	[[nodiscard]]
	std::optional<Texture> Load(const std::string& _filepath);

	/// @brief テクスチャの再読み込みを行う
	/// @param _filepath 対象のファイルパス
	/// @return 読み込んだテクスチャ
	std::optional<Texture> Reload(const std::string& _filepath, Texture* _src);


	/// @brief テクスチャ2Dの読み込みを行う
	/// @param _filepath 対象のファイルパス
	/// @return 読み込んだテクスチャ
	std::optional<Texture> Load2DTexture(const std::string& _filepath);

	/// @brief テクスチャ3Dの読み込みを行う
	/// @param _filepath 対象のファイルパス
	/// @return 読み込んだテクスチャ
	std::optional<Texture> Load3DTexture(const std::string& _filepath);


	/// @brief テクスチャ2Dの再読み込みを行う
	/// @param _filepath 対象のファイルパス
	/// @param _src 再読み込み元のテクスチャ
	/// @return 再読み込みしたテクスチャ
	std::optional<Texture> Reload2DTexture(const std::string& _filepath, Texture* _src);

	/// @brief テクスチャ3Dの再読み込みを行う
	/// @param _filepath 対象のファイルパス
	/// @param _src 再読み込み元のテクスチャ
	/// @return 再読み込みしたテクスチャ
	std::optional<Texture> Reload3DTexture(const std::string& _filepath, Texture* _src);


	/// @brief 2Dテクスチャ用のScratchImageを読み込む
	/// @param _filepath 対象のファイルパス
	/// @return 2Dテクスチャ用のScratchImage
	DirectX::ScratchImage LoadScratchImage2D(const std::string& _filepath);

	/// @brief 3Dテクスチャ用のScratchImageを読み込む
	/// @param _filepath 対象のファイルパス
	/// @return 3Dテクスチャ用のScratchImage
	DirectX::ScratchImage LoadScratchImage3D(const std::string& _filepath);


	/// @brief テクスチャのリソースを作成する(2D)
	/// @param _dxDevice DxDeviceのポインタ
	/// @param _metadata Metadata情報
	/// @return 作成したテクスチャリソース
	[[nodiscard]]
	DxResource CreateTextureResource2D(class DxDevice* _dxDevice, const DirectX::TexMetadata& _metadata);

	/// @brief テクスチャのリソースを作成する(3D)
	/// @param _dxDevice DxDeviceのポインタ
	/// @param _metadata Metadata情報
	/// @return 作成したテクスチャリソース
	[[nodiscard]]
	DxResource CreateTextureResource3D(class DxDevice* _dxDevice, const DirectX::TexMetadata& _metadata);

	/// @brief テクスチャデータをアップロードする
	/// @param _texture 対象のテクスチャリソース
	/// @param _mipScratchImage ミップマップを含むスクラッチイメージ
	/// @return アップロードに使用した中間リソース
	[[nodiscard]]
	DxResource UploadTextureData(ID3D12Resource* _texture, const DirectX::ScratchImage& _mipScratchImage);

private:

	DxManager* pDxManager_;
	AssetCollection* pAssetCollection_;

};

} /// namespace ONEngine