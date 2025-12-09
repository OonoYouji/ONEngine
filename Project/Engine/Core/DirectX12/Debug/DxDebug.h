#pragma once

/// directX
#include <d3d12.h>

/// engine
#include "../ComPtr/ComPtr.h"
#include "Engine/Core/Config/EngineConfig.h"

namespace ONEngine {

/// /////////////////////////////////////////////////
/// dx12のDebugの設定
/// /////////////////////////////////////////////////
class DxDebug final {
public:
	/// ===================================================
	/// public : method
	/// ===================================================

	DxDebug();
	~DxDebug();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="_dxDevice"></param>
	void Initialize(class DxDevice* _dxDevice);

	/// <summary>
	/// debug layerをセットする
	/// </summary>
	void SetDebugLayer();


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

#ifdef DEBUG_MODE
	ComPtr<ID3D12Debug1> debugController_ = nullptr;
#endif // DEBUG_MODE


private:
	/// ===================================================
	/// private : copy delete
	/// ===================================================

	DxDebug(const DxDebug&)            = delete;
	DxDebug(DxDebug&&)                 = delete;
	DxDebug& operator=(const DxDebug&) = delete;
	DxDebug& operator=(DxDebug&&)      = delete;
};

} /// ONEngine
