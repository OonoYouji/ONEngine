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

    finalColorBuffer_ = std::make_unique<RenderTexture>();
    finalColorBuffer_->Create(renderDevice_.get(), rtvHeap_.get(), srvHeap_.get(), windowSize, DXGI_FORMAT_R8G8B8A8_UNORM, { 0.5f, 0.7f, 0.9f, 1.0f });
    finalColorBuffer_->SetDebugName("FinalColorBuffer");
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
    finalColorBuffer_->Transition(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    
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
    
    // (Transition は Application::Render 側のポストプロセス前後で行うように変更)

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
    finalColorBuffer_->Clear(commandList);
    
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

uint32_t GraphicsEngine::ReadbackPixel(RenderTexture* texture, const Engine::Math::Vector2Int& coord) {
    if (!texture) return 0;
    auto* device = renderDevice_->GetDevice();
    auto* commandQueue = commandQueue_->GetCommandQueue();

    // 1. 専用のアロケータとコマンドリストを作成 (メインのコマンドリストを汚さないため)
    ComPtr<ID3D12CommandAllocator> tempAllocator;
    device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&tempAllocator));
    
    ComPtr<ID3D12GraphicsCommandList> tempCommandList;
    device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, tempAllocator.Get(), nullptr, IID_PPV_ARGS(&tempCommandList));

    // 2. Staging Buffer の作成 (RowPitch アライメントのため 256 バイト確保)
    D3D12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK);
    D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(256);

    ComPtr<ID3D12Resource> stagingBuffer;
    device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&stagingBuffer));

    // 3. コピーの実行
    // 元の状態から COPY_SOURCE へ遷移
    texture->Transition(tempCommandList.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE);
    
    D3D12_TEXTURE_COPY_LOCATION src = {};
    src.pResource = texture->GetResource();
    src.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    src.SubresourceIndex = 0;

    D3D12_TEXTURE_COPY_LOCATION dst = {};
    dst.pResource = stagingBuffer.Get();
    dst.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    
    // 1x1 ピクセル用のフットプリントを自前で定義
    // RowPitch は 256 バイトアライメントが必須
    dst.PlacedFootprint.Offset = 0;
    dst.PlacedFootprint.Footprint.Format = texture->GetFormat();
    dst.PlacedFootprint.Footprint.Width = 1;
    dst.PlacedFootprint.Footprint.Height = 1;
    dst.PlacedFootprint.Footprint.Depth = 1;
    dst.PlacedFootprint.Footprint.RowPitch = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT; 
    
    D3D12_BOX box = { (UINT)coord.x, (UINT)coord.y, 0, (UINT)coord.x + 1, (UINT)coord.y + 1, 1 };
    tempCommandList->CopyTextureRegion(&dst, 0, 0, 0, &src, &box);

    // 元の状態 (通常は SHADER_RESOURCE か RENDER_TARGET) に戻す
    // ※ ここではひとまず汎用的な ALL_SHADER_RESOURCE に戻す
    texture->Transition(tempCommandList.Get(), D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

    // 4. 実行と完了待機
    tempCommandList->Close();
    ID3D12CommandList* lists[] = { tempCommandList.Get() };
    commandQueue->ExecuteCommandLists(1, lists);
    
    ComPtr<ID3D12Fence> tempFence;
    device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&tempFence));
    commandQueue->Signal(tempFence.Get(), 1);
    
    HANDLE event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    tempFence->SetEventOnCompletion(1, event);
    WaitForSingleObject(event, INFINITE);
    CloseHandle(event);

    // 5. マップして読み取り
    void* mappedData = nullptr;
    D3D12_RANGE readRange = { 0, 8 }; // R32G32_UINT なら 8バイト
    stagingBuffer->Map(0, &readRange, &mappedData);
    uint32_t result = 0;
    if (mappedData) {
        result = *static_cast<uint32_t*>(mappedData);
    }
    stagingBuffer->Unmap(0, nullptr);

    return result;
}

} /// namespace Engine::Graphics
