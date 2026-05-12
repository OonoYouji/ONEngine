#include "GraphicsEngine.h"

/// directX
#include <d3d12.h>
#include <d3dx12.h>
#include "Engine/Graphics/Resource/DepthBuffer.h"
#include "Engine/Graphics/Shader/ShaderManager.h"
#include "Engine/Graphics/PostProcess/PostProcessSystem.h"
#include "Engine/Common/Console.h"

namespace Engine::Graphics {

GraphicsEngine* GraphicsEngine::instance_ = nullptr;

GraphicsEngine::GraphicsEngine() = default;
GraphicsEngine::~GraphicsEngine() {
	Shutdown();
}

void GraphicsEngine::Initialize(HWND hwnd, const Engine::Math::Vector2Int& windowSize) {
	windowSize_ = windowSize;
    hwnd_ = hwnd;

	SetDebugLayer();

	renderDevice_ = std::make_unique<RenderDevice>();
	renderDevice_->Initialize();

	CreateDebugLayer();

	commandQueue_ = std::make_unique<CommandQueue>();
	commandQueue_->Initialize(renderDevice_.get());

	rtvHeap_ = std::make_unique<DescriptorHeap>();
	rtvHeap_->Initialize(renderDevice_.get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 64, false);

	srvHeap_ = std::make_unique<DescriptorHeap>();
	srvHeap_->Initialize(renderDevice_.get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 2048, true);
	
    // 0番を ImGui 用に予約 (ここが重要)
	uint32_t imguiIndex = srvHeap_->AllocateIndex();
    Engine::Console::Log(std::format("GraphicsEngine: ImGui Descriptor Reserved at Index {}", imguiIndex));

	dsvHeap_ = std::make_unique<DescriptorHeap>();
	dsvHeap_->Initialize(renderDevice_.get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);


	for (uint32_t i = 0; i < kFrameCount; ++i) {
		frameResources_[i] = std::make_unique<FrameResource>();
		frameResources_[i]->Initialize(renderDevice_.get());
	}

	swapChain_ = std::make_unique<SwapChain>();
	swapChain_->Initialize(hwnd, windowSize);

	depthBuffer_ = std::make_unique<DepthBuffer>();
	depthBuffer_->Create(renderDevice_.get(), dsvHeap_.get(), windowSize.x, windowSize.y);

    // mainColorBuffer を 1番に作成
    mainColorBuffer_ = std::make_unique<RenderTexture>();
    mainColorBuffer_->Create(renderDevice_.get(), rtvHeap_.get(), srvHeap_.get(), windowSize, DXGI_FORMAT_R16G16B16A16_FLOAT, {0.1f, 0.1f, 0.1f, 1.0f});
    mainColorBuffer_->SetDebugName("MainColorBuffer");

    normalBuffer_ = std::make_unique<RenderTexture>();
    normalBuffer_->Create(renderDevice_.get(), rtvHeap_.get(), srvHeap_.get(), windowSize, DXGI_FORMAT_R16G16B16A16_FLOAT, {0.0f, 0.0f, 0.0f, 0.0f});
    normalBuffer_->SetDebugName("NormalBuffer");

    idBuffer_ = std::make_unique<RenderTexture>();
    idBuffer_->Create(renderDevice_.get(), rtvHeap_.get(), srvHeap_.get(), windowSize, DXGI_FORMAT_R32G32_UINT, {0.0f, 0.0f, 0.0f, 0.0f});
    idBuffer_->SetDebugName("IDBuffer");
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
    
    ID3D12DescriptorHeap* heaps[] = { srvHeap_->GetHeap() };
    commandList->SetDescriptorHeaps(_countof(heaps), heaps);

    swapChain_->BeginFrame(commandList);

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

    D3D12_VIEWPORT viewport = { 0.0f, 0.0f, (float)windowSize_.x, (float)windowSize_.y, 0.0f, 1.0f };
    D3D12_RECT scissor = { 0, 0, windowSize_.x, windowSize_.y };
    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissor);
}

void GraphicsEngine::EndFrame() {
    auto* commandList = commandQueue_->GetCommandList();
    
    mainColorBuffer_->Transition(commandList, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
    normalBuffer_->Transition(commandList, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
    idBuffer_->Transition(commandList, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = swapChain_->GetRTVHandle();
    commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
}

void GraphicsEngine::Present() {
    auto* commandList = commandQueue_->GetCommandList();
    swapChain_->EndFrame(commandList);
    commandQueue_->Execute();
    swapChain_->Present();
    frameResources_[currentFrameIndex_]->SetFenceValue(commandQueue_->Signal());
}

void GraphicsEngine::Clear(const Engine::Math::Vector4& color) {
    auto* commandList = commandQueue_->GetCommandList();
    mainColorBuffer_->Clear(commandList);
    normalBuffer_->Clear(commandList);
    idBuffer_->Clear(commandList);
    
    float backColor[] = { color.x, color.y, color.z, color.w };
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
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false);

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
