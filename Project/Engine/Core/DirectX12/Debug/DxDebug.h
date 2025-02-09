#pragma once


/// directX
#include <d3d12.h>

/// engine
#include "../ComPtr/ComPtr.h"


/// ===================================================
/// dx12のdebugの設定
/// ===================================================
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

#ifdef _DEBUG
	ComPtr<ID3D12Debug1> debugController_ = nullptr;
#endif // _DEBUG


private:

	/// ===================================================
	/// private : copy delete
	/// ===================================================

	DxDebug(const DxDebug&)            = delete;
	DxDebug(DxDebug&&)                 = delete;
	DxDebug& operator=(const DxDebug&) = delete;
	DxDebug& operator=(DxDebug&&)      = delete;

};

