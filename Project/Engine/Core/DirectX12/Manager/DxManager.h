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

/// ===================================================
/// DxObjectの管理クラス
/// ===================================================
class DxManager final {
public:
	
	/// ===================================================
	/// public : method
	/// ===================================================
	
	DxManager();
	~DxManager();
	
	void Initialize();
	void Finalize();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unique_ptr<DxDebug>        dxDebug_        = nullptr;
	std::unique_ptr<DxDevice>       dxDevice_       = nullptr;
	std::unique_ptr<DxCommand>      dxCommand_      = nullptr;
	std::unique_ptr<DxDepthStencil> dxDepthStencil_ = nullptr;

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

private:

	/// ===================================================
	/// private : copy delete
	/// ===================================================

	DxManager(const DxManager&)            = delete;
	DxManager(DxManager&&)                 = delete;
	DxManager& operator=(const DxManager&) = delete;
	DxManager& operator=(DxManager&&)      = delete;
};

