#pragma once

/// std
#include <memory>

/// engine::graphics
#include "Engine/Graphics/Utils/ComPtr.h"
#include "CommandQueue.h"
#include "RenderDevice.h"
#include "SwapChain.h"

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


	RenderDevice* GetRenderDevice() const { return renderDevice_.get(); }
	CommandQueue* GetCommandQueue() const { return commandQueue_.get(); }

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


	///
	/// 実行・同期レイヤー
	///

	std::unique_ptr<SwapChain> swapChain_ = nullptr;

};

} /// namespace Engine::Graphics