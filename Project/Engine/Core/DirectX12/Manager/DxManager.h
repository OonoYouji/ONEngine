#pragma once

/// std
#include <memory>
#include <array>

/// engine
#include "../Debug/DxDebug.h"
#include "../Device/DxDevice.h"
#include "../Command/DxCommand.h"
#include "../DescriptorHeap/DxSRVHeap.h"
#include "../DescriptorHeap/DxDSVHeap.h"
#include "../DescriptorHeap/DxRTVHeap.h"
#include "../DepthStencil/DxDepthStencil.h"

/// /////////////////////////////////////////////////
/// DxObjectの管理クラス
/// /////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class DxManager
 * @brief DirectX12のコアオブジェクト（Device, CommandList, DescriptorHeap, DepthStencil）を統括管理するクラス
 */
class DxManager final {
public:
	/// ===================================================
	/// public : method
	/// ===================================================
	
	/**
	 * @brief コンストラクタ
	 */
	DxManager();

	/**
	 * @brief デストラクタ
	 */
	~DxManager();
	
	/**
	 * @brief デバッグレイヤーの設定、デバイスの生成、コマンド関連（Queue, Allocator, List）の構築、および各種ディスクリプタヒープの確保を行います。
	 */
	void Initialize();

	/**
	 * @brief SRV/CBV/UAV用ディスクリプタヒープを現在有効なコマンドリストに設定（バインディング）します。
	 */
	void HeapBindToCommandList();

	/**
	 * @brief 指定されたユニーク名で、新しいデプスステンシルバッファを生成・追加します。
	 * @param _name デプスステンシルに割り当てる識別名
	 * @return 追加されたDxDepthStencilのポインタ
	 */
	DxDepthStencil* AddDepthStencil(const std::string& _name);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unique_ptr<DxDebug>        dxDebug_        = nullptr;
	std::unique_ptr<DxDevice>       dxDevice_       = nullptr;
	std::unique_ptr<DxCommand>      dxCommand_      = nullptr;

	std::unordered_map<std::string, size_t> depthStencilNameMap_;
	std::vector<std::unique_ptr<DxDepthStencil>> dxDepthStencils_;

	std::array<std::unique_ptr<IDxDescriptorHeap>, DescriptorHeapType_COUNT> dxDescriptorHeaps_;

public:
	/// ===================================================
	/// public : accessor
	/// ===================================================

	/**
	 * @brief DxDevice（デバイスラッパー）オブジェクトを取得します。
	 * @return DxDeviceポインタ
	 */
	DxDevice* GetDxDevice() const;

	/**
	 * @brief DxCommand（コマンドリスト・アロケータラッパー）オブジェクトを取得します。
	 * @return DxCommandポインタ
	 */
	DxCommand* GetDxCommand() const;

	/**
	 * @brief SRV用ディスクリプタヒープオブジェクト（DxSRVHeap）を取得します。
	 * @return DxSRVHeapポインタ
	 */
	DxSRVHeap* GetDxSRVHeap() const;

	/**
	 * @brief RTV用ディスクリプタヒープオブジェクト（DxRTVHeap）を取得します。
	 * @return DxRTVHeapポインタ
	 */
	DxRTVHeap* GetDxRTVHeap() const; 

	/**
	 * @brief DSV用ディスクリプタヒープオブジェクト（DxDSVHeap）を取得します。
	 * @return DxDSVHeapポインタ
	 */
	DxDSVHeap* GetDxDSVHeap() const;

	/**
	 * @brief 名前を指定して、該当するデプスステンシルバッファオブジェクトを取得します。
	 * @param _name 取得したいデプスステンシルの識別名
	 * @return DxDepthStencilポインタ（見つからない場合は nullptr）
	 */
	DxDepthStencil* GetDxDepthStencil(const std::string& _name) const;

private:
	/// ===================================================
	/// private : copy delete
	/// ===================================================

	DxManager(const DxManager&)            = delete;
	DxManager(DxManager&&)                 = delete;
	DxManager& operator=(const DxManager&) = delete;
	DxManager& operator=(DxManager&&)      = delete;
};


} /// ONEngine
