#pragma once

/// directX
#include <d3d12.h>

/// std
#include <cstdint>

/// engine
#include "../ComPtr/ComPtr.h"


/// ===================================================
/// dx12のコマンドを管理するクラス
/// ===================================================
class DxCommand {
public:

	/// ===================================================
	/// public : method
	/// ===================================================
	
	DxCommand();
	~DxCommand();

	void Initialize(class DxDevice* _dxDevice);


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	ComPtr<ID3D12CommandQueue>        commandQueue_     = nullptr;
	ComPtr<ID3D12CommandAllocator>    commandAllocator_ = nullptr;
	ComPtr<ID3D12GraphicsCommandList> commandList_      = nullptr;

	ComPtr<ID3D12Fence>               fence_            = nullptr;
	uint64_t                          fenceValue_       = 0;


public:


	/// <summary>
	/// コマンドリストを取得
	/// </summary>
	/// <returns> return : command list pointer </returns>
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); }


private:

	/// ===================================================
	/// private : copy delete
	/// ===================================================

	DxCommand(const DxCommand&) = delete;
	DxCommand(DxCommand&&) = delete;
	DxCommand& operator=(const DxCommand&) = delete;
	DxCommand& operator=(DxCommand&&) = delete;
};
