#pragma once

/// directX
#include <d3d12.h>
#include <dxgi1_6.h>

/// std
#include <vector>

/// engine
#include "../ComPtr/ComPtr.h"

/// /////////////////////////////////////////////////
/// dx12のスワップチェインを管理するクラス
/// /////////////////////////////////////////////////
class DxSwapChain {
public:

	/// @brief SwapChainのバッファ数(Double Buffering)
	static const int kBufferCount = 3;

public:
	/// ===================================================
	/// public : method
	/// ===================================================
	
	DxSwapChain();
	~DxSwapChain();
	
	/// @brief 初期化
	/// @param _dxm DxManagerのインスタンス
	/// @param _window    このSwapChainを使用するWindowのインスタンス
	void Initialize(class DxManager* _dxm, class Window* _window);

	/// @brief CommandListにViewportとScissorRectをセットする
	/// @param _commandList CommandListのポインター
	void BindViewportAndScissorRectForCommandList(ID3D12GraphicsCommandList* _commandList) const;

	/// @brief バリアを作成する
	/// @param _commandList CommandListのポインター
	/// @param _before Resourceの現在の状態
	/// @param _after Resourceの変更後の状態
	void CreateBarrier(ID3D12GraphicsCommandList* _commandList, D3D12_RESOURCE_STATES _before, D3D12_RESOURCE_STATES _after);

	/// @brief BackBufferをクリアする
	/// @param _commandList CommandListのポインター
	void ClearBackBuffer(ID3D12GraphicsCommandList* _commandList);

	/// @brief FrontBufferとBackBufferの交換
	void Present();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	ComPtr<IDXGISwapChain4>                  swapChain_  = nullptr;
	std::vector<ComPtr<ID3D12Resource>>      buffers_;
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles_;
	std::vector<uint32_t>                    rtvIndices_;

	D3D12_VIEWPORT                           viewport_;
	D3D12_RECT                               scissorRect_;

	class DxManager*                         pDxManager_ = nullptr;
	class Window*                            pWindow_    = nullptr;

};

