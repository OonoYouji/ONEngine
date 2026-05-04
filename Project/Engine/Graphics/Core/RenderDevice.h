#pragma once

/// directX
#include <d3d12.h>
#include <dxgi1_6.h>

/// D3D12MA
#include "D3D12MemAlloc.h"

/// engine::graphics
#include "Engine/Graphics/Utils/ComPtr.h"

namespace Engine::Graphics {

class RenderDevice final {
public:

	RenderDevice();
	~RenderDevice();

	void Initialize();
	void Shutdown();


	IDXGIFactory7* GetDxgiFactory() const;

	/// @brief Deviceのインスタンスの取得
	/// @return Deviceのインスタンス
	ID3D12Device* GetDevice() const;

    /// @brief Allocatorの取得
    D3D12MA::Allocator* GetAllocator() const;

private:

	ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;
	ComPtr<IDXGIAdapter4> useAdapter_ = nullptr;
	ComPtr<ID3D12Device>  device_ = nullptr;
	ComPtr<ID3D12Device10> device10_ = nullptr;

    ComPtr<D3D12MA::Allocator> allocator_ = nullptr;

};


} /// namespace Engine::Graphics