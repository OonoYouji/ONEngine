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


	void Reset(ID3D12CommandAllocator* allocator);
	void Execute();
	
	/// @brief キューにシグナルを送り、新しいフェンス値を返す
	uint64_t Signal();

	/// @brief 指定したフェンス値まで待機する
	void Wait(uint64_t fenceValue);

	/// @brief 現在の完了済みフェンス値を取得
	uint64_t GetCompletedFenceValue() const { return fence_->GetCompletedValue(); }

	ID3D12CommandQueue* GetCommandQueue() const;
	ID3D12GraphicsCommandList6* GetCommandList() const;

private:

	ComPtr<ID3D12CommandQueue>         commandQueue_;
	ComPtr<ID3D12GraphicsCommandList6> commandList_;

	ComPtr<ID3D12Fence>                fence_;
	uint64_t                           fenceValue_ = 0;
	HANDLE                             fenceEvent_ = nullptr;

};

} /// namespace Engine::Graphics