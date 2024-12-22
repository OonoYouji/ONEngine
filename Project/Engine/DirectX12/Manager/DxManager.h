#pragma once

/// std
#include <memory>
#include <array>

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

	std::unique_ptr<DxDevice>  dxDevice_  = nullptr;
	std::unique_ptr<DxCommand> dxCommand_ = nullptr;

	std::array<std::unique_ptr<IDxDescriptorHeap>, DescriptorHeapType_COUNT> dxDescriptorHeaps_;


private:

	/// ===================================================
	/// private : copy delete
	/// ===================================================

	DxManager(const DxManager&)            = delete;
	DxManager(DxManager&&)                 = delete;
	DxManager& operator=(const DxManager&) = delete;
	DxManager& operator=(DxManager&&)      = delete;
};

