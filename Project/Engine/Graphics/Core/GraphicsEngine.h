#pragma once

/// std
#include <memory>

/// engine::graphics
#include "Engine/Graphics/Utils/ComPtr.h"
#include "CommandQueue.h"
#include "RenderDevice.h"
#include "SwapChain.h"
#include "DescriptorHeap.h"
#include "FrameResource.h"
#include "Engine/Graphics/Resource/RenderTexture.h"

namespace Engine::Graphics {

///
/// DX12を使ったグラフィックスエンジン
///
class GraphicsEngine final {
	GraphicsEngine();
	~GraphicsEngine();
	GraphicsEngine(const GraphicsEngine&) = delete;
	GraphicsEngine& operator=(const GraphicsEngine&) = delete;
	GraphicsEngine(GraphicsEngine&&) = delete;
	GraphicsEngine& operator=(GraphicsEngine&&) = delete;
public:

	/// @brief インスタンスの確保
	/// @return インスタンスへの参照
	static GraphicsEngine& GetInstance() {
		static GraphicsEngine instance;
		return instance;
	}


	void Initialize(HWND hwnd, const Engine::Math::Vector2Int& windowSize);
	void Shutdown();


	void BeginFrame();
	void EndFrame();


	void Clear(const Engine::Math::Vector4& color);
	void ClearDepth();


	RenderDevice* GetRenderDevice() const { return renderDevice_.get(); }
	CommandQueue* GetCommandQueue() const { return commandQueue_.get(); }
	DescriptorHeap* GetRTVHeap() const { return rtvHeap_.get(); }
	DescriptorHeap* GetSRVHeap() const { return srvHeap_.get(); }
	DescriptorHeap* GetDSVHeap() const { return dsvHeap_.get(); }
	SwapChain* GetSwapChain() const { return swapChain_.get(); }
    class DepthBuffer* GetDepthBuffer() const { return depthBuffer_.get(); }
	RenderTexture* GetMainColorBuffer() const { return mainColorBuffer_.get(); }
	RenderTexture* GetNormalBuffer() const { return normalBuffer_.get(); }
	RenderTexture* GetIDBuffer() const { return idBuffer_.get(); }
	const Engine::Math::Vector2Int& GetWindowSize() const { return windowSize_; }

	uint32_t GetCurrentFrameIndex() const { return currentFrameIndex_; }
	FrameResource* GetCurrentFrameResource() const { return frameResources_[currentFrameIndex_].get(); }

private:

	void SetDebugLayer();
	void CreateDebugLayer();

private:

	ComPtr<ID3D12Debug1> debugController_ = nullptr;


	///
	/// 基盤レイヤー
	///

	std::unique_ptr<RenderDevice> renderDevice_ = nullptr;
	std::unique_ptr<CommandQueue> commandQueue_ = nullptr;
	std::unique_ptr<DescriptorHeap> rtvHeap_ = nullptr;
	std::unique_ptr<DescriptorHeap> srvHeap_ = nullptr;
	std::unique_ptr<DescriptorHeap> dsvHeap_ = nullptr;


	///
	/// 実行・同期レイヤー
	///

	static constexpr uint32_t kFrameCount = 3;
	std::unique_ptr<FrameResource> frameResources_[kFrameCount];
	uint32_t currentFrameIndex_ = 0;

	std::unique_ptr<SwapChain> swapChain_ = nullptr;
	std::unique_ptr<class DepthBuffer> depthBuffer_ = nullptr;
	std::unique_ptr<RenderTexture> mainColorBuffer_ = nullptr;
	std::unique_ptr<RenderTexture> normalBuffer_ = nullptr;
	std::unique_ptr<RenderTexture> idBuffer_ = nullptr;

	Engine::Math::Vector2Int windowSize_;

};

} /// namespace Engine::Graphics
