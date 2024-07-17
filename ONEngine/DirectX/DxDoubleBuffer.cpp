#include <DxDoubleBuffer.h>

#include <cassert>

#include <WinApp.h>
#include <DxDescriptor.h>
#include <DxDevice.h>

ONE::DxDoubleBuffer::DxDoubleBuffer() {}
ONE::DxDoubleBuffer::~DxDoubleBuffer() {
	swapChain_.Reset();
}


/// ===================================================
/// thisの初期化
/// ===================================================
void ONE::DxDoubleBuffer::Initialize(DxDevice* dxDevice, DxDescriptor* dxDescriptor, ID3D12CommandQueue* commandQueue) {
	InitializeSwapChain(dxDevice->GetFactory(), commandQueue);
	InitializeBuffers(dxDevice->GetDevice(), dxDescriptor);
}


/// ===================================================
/// BackBufferの色をクリア
/// ===================================================
void ONE::DxDoubleBuffer::ClearBB(ID3D12GraphicsCommandList* commandList) {
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

	commandList->OMSetRenderTargets(1, &rtvHandle_[bbIndex], false, nullptr);
	float clearColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	commandList->ClearRenderTargetView(rtvHandle_[bbIndex], clearColor, 0, nullptr);

}

void ONE::DxDoubleBuffer::Present() {
	HRESULT hr = swapChain_->Present(1, 0);
	assert(SUCCEEDED(hr));
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


/// ===================================================
/// Bufferの初期化
/// ===================================================
void ONE::DxDoubleBuffer::InitializeBuffers(ID3D12Device* device, DxDescriptor* dxDescriptor) {

	D3D12_RENDER_TARGET_VIEW_DESC desc{};
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	buffers_.resize(kBufferCount);
	rtvHandle_.resize(kBufferCount);

	for(uint8_t i = 0u; i < kBufferCount; ++i) {
		rtvHandle_[i] = dxDescriptor->GetRtvCpuHandle();
		dxDescriptor->AddRtvUsedCount();
		device->CreateRenderTargetView(buffers_[i].Get(), &desc, rtvHandle_[i]);
	}

}

