#include "GraphicsEngine.h"

/// directX
#include <d3d12.h>
#include <d3dx12.h>
#include "Engine/Graphics/Resource/DepthBuffer.h"
#include "Engine/Graphics/Shader/ShaderManager.h"
#include "Engine/Graphics/PostProcess/PostProcessSystem.h"

namespace Engine::Graphics {

GraphicsEngine* GraphicsEngine::instance_ = nullptr;

GraphicsEngine::GraphicsEngine() = default;
GraphicsEngine::~GraphicsEngine() {
	Shutdown();
}

void GraphicsEngine::Initialize(HWND hwnd, const Engine::Math::Vector2Int& windowSize) {
	windowSize_ = windowSize;
    hwnd_ = hwnd;

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
	rtvHeap_->Initialize(renderDevice_.get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 32, false);

	srvHeap_ = std::make_unique<DescriptorHeap>();
	srvHeap_->Initialize(renderDevice_.get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 2048, true); // ヒープを拡張

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

    // HDR用中間バッファの作成
    mainColorBuffer_ = std::make_unique<RenderTexture>();
    mainColorBuffer_->Create(renderDevice_.get(), rtvHeap_.get(), srvHeap_.get(), windowSize, DXGI_FORMAT_R16G16B16A16_FLOAT, {0.1f, 0.1f, 0.1f, 1.0f});

    // 法線バッファの作成
    normalBuffer_ = std::make_unique<RenderTexture>();
    normalBuffer_->Create(renderDevice_.get(), rtvHeap_.get(), srvHeap_.get(), windowSize, DXGI_FORMAT_R16G16B16A16_FLOAT, {0.0f, 0.0f, 0.0f, 0.0f});

    // ID/Flagsバッファの作成 (R32G32_UINT: EntityID, PostProcessFlags)
    idBuffer_ = std::make_unique<RenderTexture>();
    idBuffer_->Create(renderDevice_.get(), rtvHeap_.get(), srvHeap_.get(), windowSize, DXGI_FORMAT_R32G32_UINT, {0.0f, 0.0f, 0.0f, 0.0f});
}

void GraphicsEngine::Shutdown() {
    if (commandQueue_) {
        commandQueue_->Wait(commandQueue_->Signal());
    }
}

void GraphicsEngine::BeginFrame() {
    currentFrameIndex_ = (currentFrameIndex_ + 1) % kFrameCount;
    commandQueue_->Wait(frameResources_[currentFrameIndex_]->GetFenceValue());
    commandQueue_->Reset(frameResources_[currentFrameIndex_]->GetAllocator());

    auto* commandList = commandQueue_->GetCommandList();
    
    // 全員で共有するグローバル SRV ヒープをバインド
    ID3D12DescriptorHeap* heaps[] = { srvHeap_->GetHeap() };
    commandList->SetDescriptorHeaps(_countof(heaps), heaps);

    // バックバッファの遷移 (PRESENT -> RENDER_TARGET)
    swapChain_->BeginFrame(commandList);

    // MRTs の遷移 (COMMON/SRV -> RENDER_TARGET)
    mainColorBuffer_->Transition(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    normalBuffer_->Transition(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    idBuffer_->Transition(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[] = {
        mainColorBuffer_->GetRTVHandle(),
        normalBuffer_->GetRTVHandle(),
        idBuffer_->GetRTVHandle()
    };
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthBuffer_->GetDSVHandle();
    commandList->OMSetRenderTargets(3, rtvHandles, FALSE, &dsvHandle);
}

void GraphicsEngine::EndFrame() {
    auto* commandList = commandQueue_->GetCommandList();
    
    // MRTs を SHADER_RESOURCE に遷移 (ImGui で参照するため)
    mainColorBuffer_->Transition(commandList, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
    normalBuffer_->Transition(commandList, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
    idBuffer_->Transition(commandList, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

    // ImGui の描画前にバックバッファをターゲットに設定
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = swapChain_->GetRTVHandle();
    commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    // ※ ImGui_ImplDX12_RenderDrawData はこの後に Application::Run 側で呼ばれる

    // バックバッファを PRESENT に戻すのは、ImGui 描画後に行う必要がある
    // 現状は EndFrame の役割を分割するか、Application::Run 側で最終遷移を行う
}

// 最終的な書き出しと提示
void GraphicsEngine::Present() {
    auto* commandList = commandQueue_->GetCommandList();
    
    // バックバッファの最終遷移 (RENDER_TARGET -> PRESENT)
    swapChain_->EndFrame(commandList);

    commandQueue_->Execute();
    swapChain_->Present();
    frameResources_[currentFrameIndex_]->SetFenceValue(commandQueue_->Signal());
}

void GraphicsEngine::Clear(const Engine::Math::Vector4& color) {
    auto* commandList = commandQueue_->GetCommandList();
    float mainClearColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    commandList->ClearRenderTargetView(mainColorBuffer_->GetRTVHandle(), mainClearColor, 0, nullptr);
    float zero[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    commandList->ClearRenderTargetView(normalBuffer_->GetRTVHandle(), zero, 0, nullptr);
    commandList->ClearRenderTargetView(idBuffer_->GetRTVHandle(), zero, 0, nullptr);
    
    // バックバッファもクリア (ImGui の背景用)
    float backColor[] = { 0, 0, 0, 1 };
    commandList->ClearRenderTargetView(swapChain_->GetRTVHandle(), backColor, 0, nullptr);
}

void GraphicsEngine::ClearDepth() {
    auto* commandList = commandQueue_->GetCommandList();
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthBuffer_->GetDSVHandle();
    commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void GraphicsEngine::SetDebugLayer() {
#ifdef _DEBUG
	ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
	}
#endif
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
	}
}

D3D12_CPU_DESCRIPTOR_HANDLE GraphicsEngine::GetImGuiCPUHandle() const {
    return srvHeap_->GetCPUHandle(0);
}

D3D12_GPU_DESCRIPTOR_HANDLE GraphicsEngine::GetImGuiGPUHandle() const {
    return srvHeap_->GetGPUHandle(0);
}

} /// namespace Engine::Graphics
