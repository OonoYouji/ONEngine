#include "SwapChain.h"

#include <comdef.h>

/// engine
#include "Engine/Common/Assert.h"
#include "Engine/Common/Console.h"

/// engine::graphics
#include "Engine/Graphics/Core/GraphicsEngine.h"


namespace Engine::Graphics {

SwapChain::SwapChain() = default;
SwapChain::~SwapChain() = default;

void SwapChain::Initialize(HWND hwnd, const Engine::Math::Vector2Int& size) {
	GraphicsEngine& graphicsEngine = GraphicsEngine::GetInstance();
	RenderDevice* renderDevice = graphicsEngine.GetRenderDevice();
	CommandQueue* commandQueue = graphicsEngine.GetCommandQueue();


	{
		/// ---------------------------------------------------
		/// swap chain の初期化
		/// ---------------------------------------------------

		HRESULT result = S_FALSE;

		DXGI_SWAP_CHAIN_DESC1 desc{};
		desc.Width = static_cast<UINT>(size.x);
		desc.Height = static_cast<UINT>(size.y);
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = kBufferCount;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		/// SwapChain1で仮に生成
		ComPtr<IDXGISwapChain1> swapChain1;
		result = renderDevice->GetDxgiFactory()->CreateSwapChainForHwnd(
			commandQueue->GetCommandQueue(), hwnd, &desc, nullptr, nullptr, &swapChain1
		);
		if(FAILED(result)) {
			Assert(false, HrToString(result).c_str());
		}

		/// SwapChain4に引き渡す
		result = swapChain1->QueryInterface(IID_PPV_ARGS(&swapChain_));
		Assert(SUCCEEDED(result), "Failed to pass swap chain4");
	}


	//{
	//	/// ---------------------------------------------------
	//	/// buffer の初期化
	//	/// ---------------------------------------------------

	//	D3D12_RENDER_TARGET_VIEW_DESC desc{};
	//	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//	desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	//	buffers_.resize(kBufferCount);
	//	rtvHandles_.resize(kBufferCount);
	//	rtvIndices_.resize(kBufferCount);

	//	DxRTVHeap* dxRTVHeap = pDxManager_->GetDxRTVHeap();

	//	for(uint8_t i = 0u; i < kBufferCount; ++i) {
	//		HRESULT hr = swapChain_->GetBuffer(i, IID_PPV_ARGS(&buffers_[i]));
	//		Assert(SUCCEEDED(hr), "Failed to create buffer");

	//		rtvIndices_[i] = dxRTVHeap->Allocate();
	//		rtvHandles_[i] = dxRTVHeap->GetCPUDescriptorHandel(rtvIndices_[i]);

	//		pDxManager_->GetDxDevice()->GetDevice()->CreateRenderTargetView(
	//			buffers_[i].Get(), &desc, rtvHandles_[i]);
	//	}
	//}


	{
		/// ---------------------------------------------------
		/// view port, sicssor rect の初期化
		/// ---------------------------------------------------

		viewport_.Width = size.x;
		viewport_.Height = size.y;
		viewport_.TopLeftX = 0.0f;
		viewport_.TopLeftY = 0.0f;
		viewport_.MinDepth = 0.0f;
		viewport_.MaxDepth = 1.0f;

		scissorRect_.left = 0;
		scissorRect_.right = static_cast<LONG>(size.x);
		scissorRect_.top = 0;
		scissorRect_.bottom = static_cast<LONG>(size.y);
	}

	Console::Log("dx swap chain create success!!");
}


void SwapChain::Shutdown() {

}

} /// namespace Engine::Graphics