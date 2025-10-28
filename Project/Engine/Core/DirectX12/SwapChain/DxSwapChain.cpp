#include "DxSwapChain.h"

#include <comdef.h>

/// engine
#include "../Manager/DxManager.h"
#include "Engine/Core/Window/Window.h"

/// lib
#include "Engine/Core/Utility/Tools/Assert.h"
#include "Engine/Core/Utility/Tools/Log.h"
#include "Engine/Core/Config/EngineConfig.h"


DxSwapChain::DxSwapChain() {}
DxSwapChain::~DxSwapChain() {
	DxRTVHeap* dxRTVHeap = pDxManager_->GetDxRTVHeap();

	for(uint32_t index : rtvIndices_) {
		dxRTVHeap->Free(index);
	}
}


void DxSwapChain::Initialize(DxManager* _dxm, Window* _window) {

	/// 引数を保存
	pDxManager_ = _dxm;
	pWindow_    = _window;

	const Vector2& size = EngineConfig::kWindowSize;

	{
		/// ---------------------------------------------------
		/// swap chain の初期化
		/// ---------------------------------------------------

		HRESULT result = S_FALSE;

		DXGI_SWAP_CHAIN_DESC1 desc{};
		desc.Width            = static_cast<UINT>(size.x);
		desc.Height           = static_cast<UINT>(size.y);
		desc.Format           = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.BufferUsage      = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount      = kBufferCount;
		desc.SwapEffect       = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		/// SwapChain1で仮に生成
		ComPtr<IDXGISwapChain1> swapChain1;
		result = pDxManager_->GetDxDevice()->GetFactory()->CreateSwapChainForHwnd(
			pDxManager_->GetDxCommand()->GetCommandQueue(), pWindow_->GetHwnd(), &desc, nullptr, nullptr, &swapChain1
		);
		if (FAILED(result)) {
			Assert(false, HrToString(result).c_str());
		}

		/// SwapChain4に引き渡す
		result = swapChain1->QueryInterface(IID_PPV_ARGS(&swapChain_));
		Assert(SUCCEEDED(result), "Failed to pass swap chain4");
	}
	

	{
		/// ---------------------------------------------------
		/// buffer の初期化
		/// ---------------------------------------------------

		D3D12_RENDER_TARGET_VIEW_DESC desc{};
		desc.Format        = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		buffers_.resize(kBufferCount);
		rtvHandles_.resize(kBufferCount);
		rtvIndices_.resize(kBufferCount);

		DxRTVHeap* dxRTVHeap = pDxManager_->GetDxRTVHeap();

		for(uint8_t i = 0u; i < kBufferCount; ++i) {
			HRESULT hr = swapChain_->GetBuffer(i, IID_PPV_ARGS(&buffers_[i]));
			Assert(SUCCEEDED(hr), "Failed to create buffer");

			rtvIndices_[i] = dxRTVHeap->Allocate();
			rtvHandles_[i] = dxRTVHeap->GetCPUDescriptorHandel(rtvIndices_[i]);

			pDxManager_->GetDxDevice()->GetDevice()->CreateRenderTargetView(
				buffers_[i].Get(), &desc, rtvHandles_[i]);
		}
	}


	{
		/// ---------------------------------------------------
		/// view port, sicssor rect の初期化
		/// ---------------------------------------------------

		viewport_.Width     = size.x;
		viewport_.Height    = size.y;
		viewport_.TopLeftX  = 0.0f;
		viewport_.TopLeftY  = 0.0f;
		viewport_.MinDepth  = 0.0f;
		viewport_.MaxDepth  = 1.0f;

		scissorRect_.left   = 0;
		scissorRect_.right  = static_cast<LONG>(size.x);
		scissorRect_.top    = 0;
		scissorRect_.bottom = static_cast<LONG>(size.y);
	}

	Console::Log("dx swap chain create success!!");
}

void DxSwapChain::BindViewportAndScissorRectForCommandList(ID3D12GraphicsCommandList* _commandList) const {
	_commandList->RSSetViewports(1, &viewport_);
	_commandList->RSSetScissorRects(1, &scissorRect_);
}

void DxSwapChain::CreateBarrier(ID3D12GraphicsCommandList* _commandList, D3D12_RESOURCE_STATES _before, D3D12_RESOURCE_STATES _after) {
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource   = buffers_[swapChain_->GetCurrentBackBufferIndex()].Get();
	barrier.Transition.StateBefore = _before;
	barrier.Transition.StateAfter  = _after;
	_commandList->ResourceBarrier(1, &barrier);
}

void DxSwapChain::ClearBackBuffer(ID3D12GraphicsCommandList* _commandList) {
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

	DxDSVHeap*                  dxDSVHeap = pDxManager_->GetDxDSVHeap();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dxDSVHeap->GetCPUDescriptorHandel(0);

	_commandList->OMSetRenderTargets(1, &rtvHandles_[bbIndex], false, &dsvHandle);
	_commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };
	_commandList->ClearRenderTargetView(rtvHandles_[bbIndex], clearColor, 0, nullptr);
}

void DxSwapChain::Present() {
	UINT presentFlags = 0;

	HRESULT hr = swapChain_->Present(0, presentFlags);
	Assert(SUCCEEDED(hr), "Failed to present.");
}


