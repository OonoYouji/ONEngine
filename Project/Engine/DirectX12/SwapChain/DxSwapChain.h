#pragma once

/// directX
#include <d3d12.h>
#include <dxgi1_6.h>

/// std
#include <vector>

/// engine
#include "../ComPtr/ComPtr.h"

/// ===================================================
/// dx12のスワップチェインを管理するクラス
/// ===================================================
class DxSwapChain {
public:

	/// <summary>
	/// swap chainのバッファ数
	/// </summary>
	static const int kBufferCount = 2;

public:

	/// ===================================================
	/// public : method
	/// ===================================================
	
	DxSwapChain();
	~DxSwapChain();
	
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="_dxManager"> : DxManagerクラスのpointer           </param>
	/// <param name="_window">    : このswap chainを使うwindowのpointer </param>
	void Initialize(class DxManager* _dxManager, class Window* _window);

	/// <summary>
	/// command listにviewportをセットする
	/// </summary>
	/// <param name="_commandList"> : ID3D12GraphicsCommandListへのpointer </param>
	void BindViewportAndScissorRectForCommandList(ID3D12GraphicsCommandList* _commandList) const;

	/// <summary>
	/// バリアを作成する
	/// </summary>
	/// <param name="_commandList"></param>
	void CreateBarrier(ID3D12GraphicsCommandList* _commandList, D3D12_RESOURCE_STATES _before, D3D12_RESOURCE_STATES _after);

	/// <summary>
	/// バックバッファをクリアする
	/// </summary>
	/// <param name="_commandList"></param>
	void ClearBackBuffer(ID3D12GraphicsCommandList* _commandList);

	/// <summary>
	/// front bufferとback bufferの交換
	/// </summary>
	void Present();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	ComPtr<IDXGISwapChain4>                  swapChain_  = nullptr;
	std::vector<ComPtr<ID3D12Resource>>      buffers_;
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles_;
	std::vector<uint32_t>                    rtvIndices_;

	D3D12_VIEWPORT                           viewprot_;
	D3D12_RECT                               sicssorRect_;

	class DxManager*                         pDxManager_ = nullptr;
	class Window*                            pWindow_    = nullptr;

};

