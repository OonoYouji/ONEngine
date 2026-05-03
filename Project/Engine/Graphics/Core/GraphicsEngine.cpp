#include "GraphicsEngine.h"

/// directX
#include <d3d12.h>
#include "Engine/Graphics/Resource/DepthBuffer.h"


namespace Engine::Graphics {

GraphicsEngine::GraphicsEngine() = default;
GraphicsEngine::~GraphicsEngine() = default;

void GraphicsEngine::Initialize(HWND hwnd, const Engine::Math::Vector2Int& windowSize) {

	///
	/// 基盤レイヤーの初期化
	///

	SetDebugLayer();

	renderDevice_ = std::make_unique<RenderDevice>();
	renderDevice_->Initialize();

	CreateDebugLayer();

	commandQueue_ = std::make_unique<CommandQueue>();
	commandQueue_->Initialize(renderDevice_.get());

	rtvHeap_ = std::make_unique<DescriptorHeap>();
	rtvHeap_->Initialize(renderDevice_.get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, SwapChain::kBufferCount, false);

	srvHeap_ = std::make_unique<DescriptorHeap>();
	srvHeap_->Initialize(renderDevice_.get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1024, true);

	dsvHeap_ = std::make_unique<DescriptorHeap>();
	dsvHeap_->Initialize(renderDevice_.get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);


	///
	/// 実行・同期レイヤーの初期化
	///

	for (uint32_t i = 0; i < kFrameCount; ++i) {
		frameResources_[i] = std::make_unique<FrameResource>();
		frameResources_[i]->Initialize(renderDevice_.get());
	}

	swapChain_ = std::make_unique<SwapChain>();
	swapChain_->Initialize(hwnd, windowSize);

	depthBuffer_ = std::make_unique<DepthBuffer>();
	depthBuffer_->Create(renderDevice_.get(), dsvHeap_.get(), windowSize.x, windowSize.y);

}

void GraphicsEngine::Shutdown() {
    if (commandQueue_) {
        commandQueue_->Wait(commandQueue_->Signal());
    }
}

void GraphicsEngine::BeginFrame() {
    // 1. 次のフレームリソースへ
    currentFrameIndex_ = (currentFrameIndex_ + 1) % kFrameCount;

    // 2. そのフレームリソースのGPU処理が終わるまで待機
    commandQueue_->Wait(frameResources_[currentFrameIndex_]->GetFenceValue());

    // 3. コマンドリストのリセット (現在のフレームのアロケータを使用)
    commandQueue_->Reset(frameResources_[currentFrameIndex_]->GetAllocator());

    swapChain_->BeginFrame(commandQueue_->GetCommandList());

    // 深度バッファをセット
    auto rtvHandle = swapChain_->GetRTVHandle();
    auto dsvHandle = depthBuffer_->GetDSVHandle();
    commandQueue_->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
}

void GraphicsEngine::EndFrame() {
    swapChain_->EndFrame(commandQueue_->GetCommandList());
    
    // 4. コマンドの実行
    commandQueue_->Execute();
    
    // 5. 画面表示
    swapChain_->Present();
    
    // 6. 実行完了を追跡するためのシグナルを送り、フェンス値を保存
    frameResources_[currentFrameIndex_]->SetFenceValue(commandQueue_->Signal());
}

void GraphicsEngine::Clear(const Engine::Math::Vector4& color) {
	float clearColor[] = { color.x, color.y, color.z, color.w };
	commandQueue_->GetCommandList()->ClearRenderTargetView(swapChain_->GetRTVHandle(), clearColor, 0, nullptr);
}

void GraphicsEngine::ClearDepth() {
	commandQueue_->GetCommandList()->ClearDepthStencilView(depthBuffer_->GetDSVHandle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
}

void GraphicsEngine::SetDebugLayer() {
	if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController_)))) {
		debugController_->EnableDebugLayer();
		debugController_->SetEnableGPUBasedValidation(TRUE);
	}
}

void GraphicsEngine::CreateDebugLayer() {
	ComPtr<ID3D12InfoQueue> infoQueue;
	if(SUCCEEDED(renderDevice_->GetDevice()->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		D3D12_MESSAGE_ID denyIds[] = {
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		D3D12_MESSAGE_SEVERITY severtities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severtities);
		filter.DenyList.pSeverityList = severtities;

		infoQueue->PushStorageFilter(&filter);
		infoQueue.Reset();
	}
}

} /// namespace Engine::Graphics
