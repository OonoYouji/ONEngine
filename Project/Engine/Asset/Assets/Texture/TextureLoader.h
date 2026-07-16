#pragma once

/// directX
#include <d3d12.h>
#include <DirectXTex.h>

/// engine
#include "../IAssetLoader.h"
#include "Texture.h"
#include "Engine/Asset/Meta/MetaFile.h"

#include "Engine/Core/DirectX12/Resource/DxResource.h"

namespace ONEngine::Asset {
class AssetCollection;
}

namespace ONEngine::Asset {

/**
 * @class AssetLoader<Texture>
 * @brief Textureアセット（PNG, DDS等）専用のアセットローダーの特殊化クラス。DirectXTexライブラリを使用します。
 */
template<>
class AssetLoader<Texture> : public IAssetLoader {
public:

	/**
	 * @brief コンストラクタ。アセットコレクションとDirectXマネージャを受け取ります。
	 * @param _dxm DirectX12マネージャのポインタ
	 * @param _ac アセットコレクション管理クラスのポインタ
	 */
	AssetLoader(DxManager* _dxm, AssetCollection* _ac);

	/**
	 * @brief デストラクタ
	 */
	~AssetLoader() override = default;



	/**
	 * @brief ディスクからテクスチャ（PNG, DDS等）をロードします。メタデータに応じて2D/3Dの分岐を行います。
	 * @param _filepath ロード対象のファイルパス
	 * @param meta テクスチャアセットのメタデータ
	 * @return ロードされたTextureアセット（失敗時はstd::nullopt）
	 */
	[[nodiscard]]
	std::optional<Texture> Load(const std::string& _filepath, typename Meta<Texture::MetaData> meta);

	/**
	 * @brief 既存のテクスチャに対して再ロード（リロード）を実行します。
	 * @param _filepath 再ロード対象のファイルパス
	 * @param _src 再ロード元のTextureオブジェクトへのポインタ
	 * @param meta テクスチャアセットのメタデータ
	 * @return 再ロードされたTextureアセット（失敗時はstd::nullopt）
	 */
	std::optional<Texture> Reload(const std::string& _filepath, Texture* _src, typename Meta<Texture::MetaData> meta);

	/**
	 * @brief テクスチャに対応するメタデータを取得します。
	 * @param _filepath 対象アセットファイルのパス
	 * @return 解析・構築されたメタデータオブジェクト
	 */
	Meta<typename Texture::MetaData> GetMetaData(const std::string& _filepath);




	/**
	 * @brief 2Dテクスチャとしてファイルをロードします。
	 * @param _filepath 読み込み対象のファイルパス
	 * @return ロードされたTextureアセット（失敗時はstd::nullopt）
	 */
	std::optional<Texture> Load2DTexture(const std::string& _filepath);

	/**
	 * @brief 3Dテクスチャとしてファイルをロードします。
	 * @param _filepath 読み込み対象のファイルパス
	 * @return ロードされたTextureアセット（失敗時はstd::nullopt）
	 */
	std::optional<Texture> Load3DTexture(const std::string& _filepath);


	/**
	 * @brief 2Dテクスチャの再ロードを行います。
	 * @param _filepath 対象のファイルパス
	 * @param _src 再ロード元のテクスチャ
	 * @return 再ロードされたTextureオブジェクト
	 */
	std::optional<Texture> Reload2DTexture(const std::string& _filepath, Texture* _src);

	/**
	 * @brief 3Dテクスチャの再ロードを行います。
	 * @param _filepath 対象のファイルパス
	 * @param _src 再ロード元のテクスチャ
	 * @return 再ロードされたTextureオブジェクト
	 */
	std::optional<Texture> Reload3DTexture(const std::string& _filepath, Texture* _src);


	/**
	 * @brief DirectXTexを用いて、指定ファイルを2Dピクセルデータ（ScratchImage）としてロードします。
	 * @param _filepath 対象ファイルパス
	 * @return 読み込まれた ScratchImage
	 */
	DirectX::ScratchImage LoadScratchImage2D(const std::string& _filepath);

	/**
	 * @brief DirectXTexを用いて、指定ファイルを3Dピクセルデータ（ScratchImage）としてロードします。
	 * @param _filepath 対象ファイルパス
	 * @return 読み込まれた ScratchImage
	 */
	DirectX::ScratchImage LoadScratchImage3D(const std::string& _filepath);


	/**
	 * @brief DirectX12の2Dテクスチャ用GPUリソースを作成します。
	 * @param _dxDevice デバイスポインタ
	 * @param _metadata テクスチャのメタデータ
	 * @return 作成された DxResource
	 */
	[[nodiscard]]
	DxResource CreateTextureResource2D(class DxDevice* _dxDevice, const DirectX::TexMetadata& _metadata);

	/**
	 * @brief DirectX12の3Dテクスチャ用GPUリソースを作成します。
	 * @param _dxDevice デバイスポインタ
	 * @param _metadata テクスチャのメタデータ
	 * @return 作成された DxResource
	 */
	[[nodiscard]]
	DxResource CreateTextureResource3D(class DxDevice* _dxDevice, const DirectX::TexMetadata& _metadata);

	/**
	 * @brief CPU側の画素データをGPUテクスチャメモリにアップロードします。
	 * @param _texture アップロード先D3D12テクスチャリソース
	 * @param _mipScratchImage アップロード元画素データ（ScratchImage）
	 * @return コピーに使用した中間アップロードバッファの DxResource
	 */
	[[nodiscard]]
	DxResource UploadTextureData(ID3D12Resource* _texture, const DirectX::ScratchImage& _mipScratchImage);

private:

	DxManager* pDxManager_;             ///< DirectX12マネージャのポインタ
	AssetCollection* pAssetCollection_; ///< アセットコレクションのポインタ

};

} /// namespace ONEngine