#pragma once

#include <d3d12.h>
#include <cstdint>
#include "Engine/Graphics/Utils/ComPtr.h"

#include "Engine/Graphics/Resource/GpuBuffer.h"
#include "Engine/Graphics/Resource/ConstantBuffer.h"
#include "Schema/Buffers.h"
#include "Schema/Components.h"

namespace Engine::Graphics {

class RenderDevice;

///
/// 1フレーム分のリソース（コマンドアロケータ等）をまとめたクラス
///
class FrameResource {
public:
    FrameResource() = default;
    ~FrameResource() = default;

    void Initialize(RenderDevice* device);
    
    ID3D12CommandAllocator* GetAllocator() const { return commandAllocator_.Get(); }
    
    uint64_t GetFenceValue() const { return fenceValue_; }
    void SetFenceValue(uint64_t value) { fenceValue_ = value; }

    ConstantBuffer* GetSceneCB() { return &sceneCB_; }

private:
    ComPtr<ID3D12CommandAllocator> commandAllocator_;
    uint64_t fenceValue_ = 0;

    // フレームごとの定数バッファ
    ConstantBuffer sceneCB_;
};

} // namespace Engine::Graphics
