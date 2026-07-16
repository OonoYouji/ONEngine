#pragma once

/// directX
#include <d3d12.h>

/// std
#include <cstdint>

/// engine
#include "../ComPtr/ComPtr.h"

namespace ONEngine {

/// /////////////////////////////////////////////////
/// depth stencil class
/// /////////////////////////////////////////////////
/**
 * @class DxDepthStencil
 * @brief 深度ステンシルバッファリソース（DSVおよびシェーダ参照用SRV）を生成・管理し、リソースバリア遷移を制御するクラス
 */
class DxDepthStencil final {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================
	
	/**
	 * @brief コンストラクタ
	 */
	DxDepthStencil();

	/**
	 * @brief デストラクタ
	 */
	~DxDepthStencil();

	/**
	 * @brief 深度ステンシルテクスチャリソースを作成し、DSVヒープおよびSRVヒープへビューを登録・初期化します。
	 * @param _dxDevice デバイスオブジェクトへのポインタ
	 * @param _dxDsvHeap DSVを割り当てるディスクリプタヒープポインタ
	 * @param _dxSrvHeap SRVを割り当てるディスクリプタヒープポインタ
	 */
	void Initialize(class DxDevice* _dxDevice, class DxDSVHeap* _dxDsvHeap, class DxSRVHeap* _dxSrvHeap);


	/**
	 * @brief 深度バッファをピクセルシェーダで参照可能なステート（PIXEL_SHADER_RESOURCE）に遷移するリソースバリアを積みます。
	 * @param _cmdList 対象とするコマンドリストポインタ
	 */
	void CreateBarrierPixelShaderResource(ID3D12GraphicsCommandList* _cmdList);

	/**
	 * @brief 深度バッファを書き込み可能ステート（DEPTH_WRITE）に遷移するリソースバリアを積みます。
	 * @param _cmdList 対象とするコマンドリストポインタ
	 */
	void CreateBarrierDepthWrite(ID3D12GraphicsCommandList* _cmdList);


	/**
	 * @brief 深度リソースのSRVディスクリプタインデックスを取得します。
	 * @return ディスクリプタヒープ内のインデックス
	 */
	uint32_t GetDepthSrvHandle() const;

	/**
	 * @brief 深度リソースのDSVディスクリプタインデックスを取得します。
	 * @return ディスクリプタヒープ内のインデックス
	 */
	uint32_t GetDepthDsvHandle() const;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	ComPtr<ID3D12Resource> depthStencilResource_;
	D3D12_RESOURCE_STATES currentResourceState_;

	uint32_t depthSrvHandle_;
	uint32_t depthDsvHandle_;

};


} /// ONEngine
