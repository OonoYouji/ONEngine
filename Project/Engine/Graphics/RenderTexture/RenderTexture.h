#pragma once

/// engine
#include "Engine/Asset/Assets/Texture/Texture.h"
#include "Engine/Core/DirectX12/Resource/DxResource.h"
#include "Engine/Core/Utility/Math/Vector4.h"

namespace ONEngine {
class DxManager;
class DxDepthStencil;
class DxCommand;
class DxDSVHeap;
class RenderTexture;
}

namespace ONEngine::Asset {
class AssetCollection;
}



/// ///////////////////////////////////////////////////
/// render texture
/// ///////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class RenderTexture
 * @brief レンダーターゲット（RTV）およびシェーダリソース（SRV）として機能するDX12テクスチャを管理するクラス
 */
class RenderTexture {
private:
	/// ===================================================
	/// private : sub class
	/// ===================================================

	struct Handle {
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	RenderTexture();

	/**
	 * @brief デストラクタ
	 */
	~RenderTexture();

	/**
	 * @brief レンダーターゲットテクスチャリソースを生成し、RTVとSRVへのアロケーション・バインドを行います。
	 * @param _format ピクセルフォーマット
	 * @param _clearColor クリア色（RGBA）
	 * @param _textureSize ピクセル解像度
	 * @param _name デバッグ用のテクスチャ名称
	 * @param _dxm DX12システムマネージャ
	 * @param _dxDepthStencil 対応する深度ステンシルオブジェクト
	 * @param _assetCollection アセット登録を行うコレクション
	 */
	void Initialize(DXGI_FORMAT _format, const Vector4& _clearColor, const Vector2& _textureSize, const std::string& _name, DxManager* _dxm, DxDepthStencil* _dxDepthStencil, Asset::AssetCollection* _assetCollection);

	/**
	 * @brief 単一のレンダーターゲットとしてコマンドリストに設定します。
	 * @param _dxCommand コマンドリスト
	 * @param _dxDSVHeap DSVを保持するディスクリプタヒープ
	 * @param _clear trueの場合、設定と同時にクリア色でバッファをクリアします
	 */
	void SetRenderTarget(DxCommand* _dxCommand, DxDSVHeap* _dxDSVHeap, bool _clear = true);

	/**
	 * @brief 複数（MRT）のレンダーターゲットとしてコマンドリストに一括設定します。
	 * @param _dxCommand コマンドリスト
	 * @param _dxDSVHeap DSVを保持するディスクリプタヒープ
	 * @param _other 同時にバインドする他レンダーターゲットのリスト
	 * @param _clear trueの場合、設定と同時に全バッファをクリアします
	 */
	void SetRenderTarget(DxCommand* _dxCommand, DxDSVHeap* _dxDSVHeap, const std::vector<std::unique_ptr<RenderTexture>>& _other, bool _clear = true);

	/**
	 * @brief リソースのステートをRENDER_TARGETへ移行するバリアを積みます。
	 * @param _dxCommand コマンドリスト
	 */
	void CreateBarrierRenderTarget(DxCommand* _dxCommand);

	/**
	 * @brief リソースのステートをシェーダ読み取り可能な状態（PIXEL_SHADER_RESOURCE）へ移行するバリアを積みます。
	 * @param _dxCommand コマンドリスト
	 */
	void CreateBarrierPixelShaderResource(DxCommand* _dxCommand);

	/**
	 * @brief このレンダーターゲットテクスチャの識別名を取得します。
	 * @return 識別名文字列
	 */
	const std::string& GetName() const;

	/**
	 * @brief 内部テクスチャのDX12リソースオブジェクトを取得します。
	 * @return DxResourceの参照
	 */
	DxResource& GetDxResource();

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	DxDepthStencil* pDxDepthStencil_ = nullptr;

	std::string name_;
	Vector4 clearColor_;

	Handle rtvHandle_;
	Asset::Texture* texture_ = nullptr;


};



/// ///////////////////////////////////////////////////
/// UAVTexture
/// ///////////////////////////////////////////////////
/**
 * @class UAVTexture
 * @brief 書込み可能なアンオーダードアクセスビュー（UAV）をカプセル化してアセット登録・管理を行うクラス
 */
class UAVTexture {
private:
	/// ===================================================
	/// private : sub class
	/// ===================================================

	struct Handle {
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
	};

public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	UAVTexture();

	/**
	 * @brief デストラクタ
	 */
	~UAVTexture();

	/**
	 * @brief UAV用テクスチャの初期化とアセット登録を行います。
	 * @param _textureName テクスチャアセット名
	 * @param _dxm DX12システムマネージャ
	 * @param _assetCollection アセット登録用コレクション
	 */
	void Initialize(const std::string& _textureName, DxManager* _dxm, class Asset::AssetCollection* _assetCollection);


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	Asset::Texture* texture_ = nullptr;

};

} /// ONEngine
