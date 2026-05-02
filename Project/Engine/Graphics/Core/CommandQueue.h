#pragma once

/// directX
#include <d3d12.h>

/// std
#include <cstdint>

/// engine::graphics
#include "Engine/Graphics/Utils/ComPtr.h"

namespace Engine::Graphics {
class RenderDevice;
}

namespace Engine::Graphics {

///
/// DX12のコマンドキューを管理するクラス
///
class CommandQueue final {
public:

	CommandQueue();
	~CommandQueue();

	void Initialize(RenderDevice* renderDevice);
	void Shutdown();


	ID3D12CommandQueue* GetCommandQueue() const;

private:

	ComPtr<ID3D12CommandQueue>         commandQueue_;
	ComPtr<ID3D12CommandAllocator>     commandAllocator_;
	ComPtr<ID3D12GraphicsCommandList6> commandList_;

	ComPtr<ID3D12Fence>                fence_;
	uint64_t                           fenceValue_;

};

} /// namespace Engine::Graphics