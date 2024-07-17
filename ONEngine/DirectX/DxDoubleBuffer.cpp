#include <DxDoubleBuffer.h>

#include <cassert>

#include <WinApp.h>


ONE::DxDoubleBuffer::DxDoubleBuffer() {}
ONE::DxDoubleBuffer::~DxDoubleBuffer() {
	swapChain_.Reset();
}


/// ===================================================
/// thisの初期化
/// ===================================================
void ONE::DxDoubleBuffer::Initialize(IDXGIFactory7* factory, ID3D12CommandQueue* commandQueue) {
	InitializeSwapChain(factory, commandQueue);
}


/// ===================================================
/// swapChainの初期化
/// ===================================================
void ONE::DxDoubleBuffer::InitializeSwapChain(IDXGIFactory7* factory, ID3D12CommandQueue* commandQueue) {
	HRESULT result = S_FALSE;

	DXGI_SWAP_CHAIN_DESC1 desc{};
	desc.Width = WinApp::kWindowSizeX;
	desc.Height = WinApp::kWindowSizeY;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = kBufferCount;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	///- SwapChain1で仮に生成
	ComPtr<IDXGISwapChain1> swapChain1;
	result = factory->CreateSwapChainForHwnd(
		commandQueue, WinApp::GetInstance()->GetHWND(), &desc, nullptr, nullptr, &swapChain1
	);
	assert(SUCCEEDED(result));

	///- SwapChain4に引き渡す
	result = swapChain1->QueryInterface(IID_PPV_ARGS(&swapChain_));
	assert(SUCCEEDED(result));
}
