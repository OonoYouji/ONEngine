#pragma once

#include <d3d12.h>
#include <cstdint>
#include "D3D12MemAlloc.h"
#include "Engine/Graphics/Utils/ComPtr.h"
#include "Engine/Common/Assert.h"

namespace Engine::Graphics {

class RenderDevice;

///
/// GPU定数バッファを管理するクラス
///
class ConstantBuffer {
public:
    ConstantBuffer();
    ~ConstantBuffer();

    /// @brief 定数バッファの作成
    /// @param device デバイス
    /// @param size バッファサイズ（バイト単位。内部で256バイトアライメントされる）
    void Create(RenderDevice* device, uint32_t size);

    /// @brief データの更新
    /// @param data データへのポインタ
    /// @param size データサイズ
    /// @param offset 書き込みオフセット
    void Update(const void* data, uint32_t size, uint32_t offset = 0);

    ID3D12Resource* GetResource() const { return allocation_->GetResource(); }
    D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return allocation_->GetResource()->GetGPUVirtualAddress(); }

private:
    ComPtr<D3D12MA::Allocation> allocation_;
    void* mappedData_ = nullptr;
    uint32_t bufferSize_ = 0;
};

} // namespace Engine::Graphics
