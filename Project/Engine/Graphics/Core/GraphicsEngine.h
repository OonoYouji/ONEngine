#pragma once

/// std
#include <memory>

/// engine::graphics
#include "Engine/Graphics/Utils/ComPtr.h"
#include "Engine/Graphics/Core/CommandQueue.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Graphics/Core/SwapChain.h"
#include "Engine/Graphics/Core/DescriptorHeap.h"
#include "Engine/Graphics/Core/FrameResource.h"
#include "Engine/Graphics/Resource/RenderTexture.h"
#include "Engine/Core/Math/Math.h"

namespace Engine::Graphics {

///
/// エンジンのグラフィックス機能全体を管理するクラス
///
class GraphicsEngine {
	GraphicsEngine(const GraphicsEngine&) = delete;
	GraphicsEngine& operator=(const GraphicsEngine&) = delete;
	GraphicsEngine(GraphicsEngine&&) = delete;
	GraphicsEngine& operator=(GraphicsEngine&&) = delete;
public:

	static GraphicsEngine& GetInstance() {
		return *instance_;
	}

	static void CreateInstance() {
		if (!instance_) instance_ = new GraphicsEngine();
	}

	static void DestroyInstance() {
		delete instance_;
		instance_ = nullptr;
	}

	void Initialize(HWND hwnd, const Engine::Math::Vector2Int& windowSize);
	void Shutdown();

	void BeginFrame();
	void EndFrame();
	void Present();

	void Clear(const Engine::Math::Vector4& color);
	void ClearDepth();

	RenderDevice* GetRenderDevice() const { return renderDevice_.get(); }
	CommandQueue* GetCommandQueue() const { return commandQueue_.get(); }
	SwapChain*    GetSwapChain()    const { return swapChain_.get(); }
	
	DescriptorHeap* GetSRVHeap()    const { return srvHeap_.get(); }
	DescriptorHeap* GetRTVHeap()    const { return rtvHeap_.get(); }
	DescriptorHeap* GetDSVHeap()    const { return dsvHeap_.get(); }
	
	FrameResource* GetCurrentFrameResource() const { return frameResources_[currentFrameIndex_].get(); }
	uint32_t GetCurrentFrameIndex() const { return currentFrameIndex_; }

	RenderTexture* GetMainColorBuffer() const { return mainColorBuffer_.get(); }
	RenderTexture* GetNormalBuffer()    const { return normalBuffer_.get(); }
	RenderTexture* GetIDBuffer()        const { return idBuffer_.get(); }
	RenderTexture* GetFinalColorBuffer() const { return finalColorBuffer_.get(); }

	D3D12_CPU_DESCRIPTOR_HANDLE GetImGuiCPUHandle() const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetImGuiGPUHandle() const;

	HWND GetHWND() const { return hwnd_; }
	Engine::Math::Vector2Int GetWindowSize() const { return windowSize_; }

private:
	GraphicsEngine();
	~GraphicsEngine();

	static GraphicsEngine* instance_;

	HWND hwnd_;

	std::unique_ptr<RenderDevice> renderDevice_ = nullptr;
	std::unique_ptr<CommandQueue> commandQueue_ = nullptr;
	
	std::unique_ptr<DescriptorHeap> rtvHeap_ = nullptr;
	std::unique_ptr<DescriptorHeap> srvHeap_ = nullptr;
	std::unique_ptr<DescriptorHeap> dsvHeap_ = nullptr;

	static constexpr uint32_t kFrameCount = 3;
	std::unique_ptr<FrameResource> frameResources_[kFrameCount];
	uint32_t currentFrameIndex_ = 0;

	std::unique_ptr<SwapChain> swapChain_ = nullptr;
	std::unique_ptr<class DepthBuffer> depthBuffer_ = nullptr;
	std::unique_ptr<RenderTexture> mainColorBuffer_ = nullptr;
	std::unique_ptr<RenderTexture> normalBuffer_ = nullptr;
	std::unique_ptr<RenderTexture> idBuffer_ = nullptr;
	std::unique_ptr<RenderTexture> finalColorBuffer_ = nullptr;

	Engine::Math::Vector2Int windowSize_;

	void SetDebugLayer();
	void CreateDebugLayer();

};

} /// namespace Engine::Graphics
