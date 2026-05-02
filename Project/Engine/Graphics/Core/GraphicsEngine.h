#pragma once

/// std
#include <memory>

/// engine::graphics
#include "CommandQueue.h"
#include "RenderDevice.h"
#include "Engine/Graphics/Utils/ComPtr.h"


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


	void Initialize();
	void Shutdown();


private:

	void SetDebugLayer();
	void CreateDebugLayer();

private:

	ComPtr<ID3D12Debug1> debugController_ = nullptr;


	std::unique_ptr<RenderDevice> renderDevice_ = nullptr;
	std::unique_ptr<CommandQueue> commandQueue_ = nullptr;



};

} /// namespace Engine::Graphics