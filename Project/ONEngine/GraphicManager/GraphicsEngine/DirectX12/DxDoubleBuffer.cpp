#include "DxDoubleBuffer.h"

#include <cassert>

#include <Core/ONEngine.h>

#include "WindowManager/WinApp.h"

#include "DxCommon.h"
#include "DxCommand.h"
#include "DxDescriptor.h"
#include "DxBarrierCreator.h"
#include "DxDevice.h"


ONE::DxDoubleBuffer::DxDoubleBuffer() {}
ONE::DxDoubleBuffer::~DxDoubleBuffer() {
	swapChain_.Reset();
}


/// ===================================================
/// thisの初期化
/// ===================================================
void ONE::DxDoubleBuffer::Initialize(
	WinApp* winApp, DxDevice* dxDevice,
	DxDescriptorHeap<HeapType::RTV>* _rtvDescriptorHeap,
	DxDescriptorHeap<HeapType::DSV>* _dsvDescriptorHeap, 
	ID3D12CommandQueue* commandQueue) {

	pWinApp_ = winApp;
	pDSVDescriptorHeap_ = _dsvDescriptorHeap;

	InitializeSwapChain(dxDevice->GetFactory(), commandQueue);
	InitializeBuffers(dxDevice->GetDevice(), _rtvDescriptorHeap);

	viewprot_.Width = ONE::WinApp::kWindowSizeX;
	viewprot_.Height = ONE::WinApp::kWindowSizeY;
	viewprot_.TopLeftX = 0;
	viewprot_.TopLeftY = 0;
	viewprot_.MinDepth = 0.0f;
	viewprot_.MaxDepth = 1.0f;

	sicssorRect_.left = 0;
	sicssorRect_.right = ONE::WinApp::kWindowSizeX;
	sicssorRect_.top = 0;
	sicssorRect_.bottom = ONE::WinApp::kWindowSizeY;


}


/// ===================================================
/// BackBufferの色をクリア
/// ===================================================
void ONE::DxDoubleBuffer::ClearBB(ID3D12GraphicsCommandList* commandList) {
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = pDSVDescriptorHeap_->GetCPUDescriptorHandel(0);

	commandList->OMSetRenderTargets(1, &rtvHandle_[bbIndex], false, &dsvHandle);
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };
	commandList->ClearRenderTargetView(rtvHandle_[bbIndex], clearColor, 0, nullptr);
}


/// ===================================================
/// 画面の交換
/// ===================================================
void ONE::DxDoubleBuffer::Present() {
	HRESULT hr = swapChain_->Present(1, 0);
	assert(SUCCEEDED(hr));
}


/// ===================================================
/// backBufferにバリアーの生成
/// ===================================================
void ONE::DxDoubleBuffer::CreateBarrier(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) {
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = buffers_[swapChain_->GetCurrentBackBufferIndex()].Get();
	barrier.Transition.StateBefore = before;
	barrier.Transition.StateAfter = after;
	commandList->ResourceBarrier(1, &barrier);
}

void ONE::DxDoubleBuffer::SetViewport(ID3D12GraphicsCommandList* commandList) {
	commandList->RSSetViewports(1, &viewprot_);
}

void ONE::DxDoubleBuffer::SetSiccorRect(ID3D12GraphicsCommandList* commandList) {
	commandList->RSSetScissorRects(1, &sicssorRect_);
}

void ONE::DxDoubleBuffer::SetRanderTarget(DxCommand* command) {
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();
	ID3D12GraphicsCommandList* commandList = command->GetList();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = pDSVDescriptorHeap_->GetCPUDescriptorHandel(0);
	commandList->OMSetRenderTargets(1, &rtvHandle_[bbIndex], false, &dsvHandle);
}


void ONE::DxDoubleBuffer::CopyToBB(ID3D12GraphicsCommandList* commnadList, ID3D12Resource* resource, D3D12_RESOURCE_STATES current) {
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

	ONE::DxBarrierCreator::CreateBarrier(resource, current, D3D12_RESOURCE_STATE_COPY_SOURCE);
	ONE::DxBarrierCreator::CreateBarrier(buffers_[bbIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_DEST);

	commnadList->CopyResource(buffers_[bbIndex].Get(), resource);

	ONE::DxBarrierCreator::CreateBarrier(resource, D3D12_RESOURCE_STATE_COPY_SOURCE, current);
	ONE::DxBarrierCreator::CreateBarrier(buffers_[bbIndex].Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET);
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
		commandQueue, pWinApp_->GetHWND(), &desc, nullptr, nullptr, &swapChain1
	);
	assert(SUCCEEDED(result));

	///- SwapChain4に引き渡す
	result = swapChain1->QueryInterface(IID_PPV_ARGS(&swapChain_));
	assert(SUCCEEDED(result));
}


/// ===================================================
/// Bufferの初期化
/// ===================================================
void ONE::DxDoubleBuffer::InitializeBuffers(ID3D12Device* device, DxDescriptorHeap<HeapType::RTV>* _rtvDescriptorHeap) {

	D3D12_RENDER_TARGET_VIEW_DESC desc{};
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	buffers_.resize(kBufferCount);
	rtvHandle_.resize(kBufferCount);

	for(uint8_t i = 0u; i < kBufferCount; ++i) {
		HRESULT hr = swapChain_->GetBuffer(i, IID_PPV_ARGS(&buffers_[i]));
		assert(SUCCEEDED(hr));

		uint32_t index = _rtvDescriptorHeap->Allocate();
		rtvHandle_[i] = _rtvDescriptorHeap->GetCPUDescriptorHandel(index);

		device->CreateRenderTargetView(buffers_[i].Get(), &desc, rtvHandle_[i]);
	}

}

