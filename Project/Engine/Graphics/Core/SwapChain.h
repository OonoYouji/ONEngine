#pragma once

/// directX
#include <d3d12.h>
#include <dxgi1_6.h>

/// std
#include <vector>

/// engine::math
#include "Engine/Core/Math/Math.h"

/// engine::graphics
#include "Engine/Graphics/Utils/ComPtr.h"

namespace Engine::Graphics {

///
/// DX12のスワップチェインを管理するクラス
///
class SwapChain final {
public:

	static constexpr uint32_t kBufferCount = 2;

public:

	SwapChain();
	~SwapChain();

	void Initialize(HWND hwnd, const Engine::Math::Vector2Int& size);
	void Shutdown();

private:

	ComPtr<IDXGISwapChain4> swapChain_;
	std::vector<ComPtr<ID3D12Resource>> buffers_;
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles_;
	std::vector<uint32_t> rtvIndices_;

	D3D12_VIEWPORT viewport_;
	D3D12_RECT scissorRect_;
};

} /// namespace Engine::Graphics