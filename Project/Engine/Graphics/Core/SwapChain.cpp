#include "SwapChain.h"

#include <comdef.h>
#include <d3dx12.h>

/// engine
#include "Engine/Common/Assert.h"
#include "Engine/Common/Console.h"

/// engine::graphics
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Graphics/Core/DescriptorHeap.h"

namespace Engine::Graphics {

SwapChain::SwapChain() = default;
SwapChain::~SwapChain() = default;

void SwapChain::Initialize(HWND hwnd, const Engine::Math::Vector2Int& windowSize) {
	HRESULT result = S_FALSE;

	auto* graphics = &GraphicsEngine::GetInstance();
	auto* device = graphics->GetRenderDevice();

	/// スワップチェーンの設定
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = windowSize.x;
	swapChainDesc.Height = windowSize.y;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 3;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

	ComPtr<IDXGISwapChain1> swapChain1;
	result = device->GetDxgiFactory()->CreateSwapChainForHwnd(
		graphics->GetCommandQueue()->GetCommandQueue(),
		hwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain1
	);
	Assert(SUCCEEDED(result), "Failed to create swap chain.");

	result = swapChain1.As(&swapChain_);
	Assert(SUCCEEDED(result), "Failed to cast swap chain.");

	/// バックバッファの取得
	buffers_.resize(swapChainDesc.BufferCount);
	rtvHandles_.resize(swapChainDesc.BufferCount);

	auto* rtvHeap = graphics->GetRTVHeap();

	for (uint32_t i = 0; i < swapChainDesc.BufferCount; ++i) {
		result = swapChain_->GetBuffer(i, IID_PPV_ARGS(&buffers_[i]));
		Assert(SUCCEEDED(result), "Failed to get back buffer.");

		rtvHandles_[i] = rtvHeap->Allocate();
		device->GetDevice()->CreateRenderTargetView(buffers_[i].Get(), nullptr, rtvHandles_[i]);
        
        std::wstring name = L"BackBuffer" + std::to_wstring(i);
        buffers_[i]->SetName(name.c_str());
	}

	viewport_ = { 0.0f, 0.0f, (float)windowSize.x, (float)windowSize.y, 0.0f, 1.0f };
	scissorRect_ = { 0, 0, windowSize.x, windowSize.y };

	Console::Log("SwapChain: Initialized successfully.");
}

void SwapChain::BeginFrame(ID3D12GraphicsCommandList* commandList) {
	uint32_t backBufferIndex = GetCurrentBackBufferIndex();

	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(buffers_[backBufferIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList->ResourceBarrier(1, &barrier);

	commandList->RSSetViewports(1, &viewport_);
	commandList->RSSetScissorRects(1, &scissorRect_);
}

void SwapChain::EndFrame(ID3D12GraphicsCommandList* commandList) {
	uint32_t backBufferIndex = GetCurrentBackBufferIndex();

	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(buffers_[backBufferIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	commandList->ResourceBarrier(1, &barrier);
}

void SwapChain::Present() {
	HRESULT hr = swapChain_->Present(1, 0);
    if (FAILED(hr)) {
        Console::LogError("SwapChain: Present Failed!");
    }
}

uint32_t SwapChain::GetCurrentBackBufferIndex() const {
	return swapChain_->GetCurrentBackBufferIndex();
}

D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::GetRTVHandle() const {
	return rtvHandles_[GetCurrentBackBufferIndex()];
}

} /// namespace Engine::Graphics
