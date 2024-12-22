#pragma once

/// std
#include <memory>

#include "../Device/DxDevice.h"


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

	std::unique_ptr<DxDevice> dxDevice_ = nullptr;


private:

	/// ===================================================
	/// private : copy delete
	/// ===================================================

	DxManager(const DxManager&)            = delete;
	DxManager(DxManager&&)                 = delete;
	DxManager& operator=(const DxManager&) = delete;
	DxManager& operator=(DxManager&&)      = delete;
};

