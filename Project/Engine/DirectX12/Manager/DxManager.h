#pragma once

/// std
#include <memory>
#include <array>

/// engine
#include "../Debug/DxDebug.h"
#include "../Device/DxDevice.h"
#include "../Command/DxCommand.h"
#include "../DescriptorHeap/DxDescriptorHeap.h"


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

	std::unique_ptr<DxDebug>   dxDebug_   = nullptr;
	std::unique_ptr<DxDevice>  dxDevice_  = nullptr;
	std::unique_ptr<DxCommand> dxCommand_ = nullptr;

	std::array<std::unique_ptr<IDxDescriptorHeap>, DescriptorHeapType_COUNT> dxDescriptorHeaps_;


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// <summary>
	/// dxDeviceを取得
	/// </summary>
	/// <returns></returns>
	DxDevice* GetDxDevice() const { return dxDevice_.get(); }

	/// <summary>
	/// dxCommandを取得
	/// </summary>
	/// <returns></returns>
	DxCommand* GetDxCommand() const { return dxCommand_.get(); }

	/// <summary>
	/// dxDescriptorHeapを取得
	/// </summary>
	/// <param name="_type"> : descriptor heapのtype               </param>
	/// <returns>              return : 引数のtypeのdescriptor heap </returns>
	IDxDescriptorHeap* GetDxDescriptorHeap(DescriptorHeapType _type) const { return dxDescriptorHeaps_[_type].get(); }

private:

	/// ===================================================
	/// private : copy delete
	/// ===================================================

	DxManager(const DxManager&)            = delete;
	DxManager(DxManager&&)                 = delete;
	DxManager& operator=(const DxManager&) = delete;
	DxManager& operator=(DxManager&&)      = delete;
};

