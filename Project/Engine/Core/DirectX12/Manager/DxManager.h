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

class DxManager final {
public:
	/// ===================================================
	/// public : method
	/// ===================================================
	
	DxManager();
	~DxManager();
	
	/// @brief 初期化
	void Initialize();

	/// @brief SRVHeapをCommandListにバインドする
	void HeapBindToCommandList();

	/// @brief 追加のDepthStencilを作成する
	/// @return 作成されたDepthStencilのポインタ
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

	/// @brief DxDeviceのインスタンスの取得
	/// @return DxDeviceインスタンス
	DxDevice* GetDxDevice() const;

	/// @brief DxCommandのインスタンスの取得
	/// @return DxCommandインスタンス
	DxCommand* GetDxCommand() const;

	/// @brief DxSRVHeapのインスタンスの取得
	/// @return DxSRVHeapインスタンス
	DxSRVHeap* GetDxSRVHeap() const;

	/// @brief DxRTVHeapのインスタンスの取得
	/// @return DxRTVHeapインスタンス
	DxRTVHeap* GetDxRTVHeap() const; 

	/// @brief DxDSVHeapのインスタンスの取得
	/// @return DxDSVHeapインスタンス
	DxDSVHeap* GetDxDSVHeap() const;

	/// @brief 名前からDxDepthStencilを取得する
	/// @param _name DxDepthStencilの名前
	/// @return 見つかったDxDepthStencilのポインタ、見つからなかった場合はnullptr
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
