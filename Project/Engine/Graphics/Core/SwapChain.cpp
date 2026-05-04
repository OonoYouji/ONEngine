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


	{
		/// ---------------------------------------------------
		/// buffer の初期化
		/// ---------------------------------------------------

		buffers_.resize(kBufferCount);
		rtvHandles_.resize(kBufferCount);

		DescriptorHeap* rtvHeap = graphicsEngine.GetRTVHeap();

		for(uint32_t i = 0u; i < kBufferCount; ++i) {
			HRESULT hr = swapChain_->GetBuffer(i, IID_PPV_ARGS(&buffers_[i]));
			Assert(SUCCEEDED(hr), "Failed to create buffer");

			rtvHandles_[i] = rtvHeap->Allocate();

			renderDevice->GetDevice()->CreateRenderTargetView(
				buffers_[i].Get(), nullptr, rtvHandles_[i]);
		}
	}


	{
		/// ---------------------------------------------------
		/// view port, sicssor rect の初期化
		/// ---------------------------------------------------

		viewport_.Width = static_cast<float>(size.x);
		viewport_.Height = static_cast<float>(size.y);
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

void SwapChain::BeginFrame(ID3D12GraphicsCommandList* commandList) {
	uint32_t backBufferIndex = GetCurrentBackBufferIndex();

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = buffers_[backBufferIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList->ResourceBarrier(1, &barrier);

	commandList->RSSetViewports(1, &viewport_);
	commandList->RSSetScissorRects(1, &scissorRect_);

	//D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = GetRTVHandle();
	//commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
}

void SwapChain::EndFrame(ID3D12GraphicsCommandList* commandList) {
	uint32_t backBufferIndex = GetCurrentBackBufferIndex();

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = buffers_[backBufferIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList->ResourceBarrier(1, &barrier);
}

void SwapChain::Present() {
	swapChain_->Present(1, 0);
}

uint32_t SwapChain::GetCurrentBackBufferIndex() const {
	return swapChain_->GetCurrentBackBufferIndex();
}

D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::GetRTVHandle() const {
	return rtvHandles_[GetCurrentBackBufferIndex()];
}

} /// namespace Engine::Graphics