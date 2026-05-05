#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include "EcsWorld.h"
#include "GeneratedComponents.h"

using Microsoft::WRL::ComPtr;

class EcsRenderer {
    ComPtr<ID3D12Device> device;
    ComPtr<ID3D12Resource> gpu_buffer;
    void* mapped_data = nullptr;
    size_t buffer_capacity = 0;

public:
    EcsRenderer(ID3D12Device* d) : device(d) {}

    /**
     * @brief ECSのチャンクデータをGPUバッファへアップロードします。
     * 設計書 v6.1 2.2項 [GPU Upload] フェーズの実装。
     */
    void upload_transforms(EcsWorld& world) {
        auto storage = world.get_storage<Transform>();
        size_t total_count = storage->size();
        if (total_count == 0) return;

        // バッファサイズが不足していれば再確保 (本来は二重/三重バッファリングが必要)
        size_t required_size = total_count * sizeof(Transform);
        if (required_size > buffer_capacity) {
            recreate_buffer(required_size);
        }

        // チャンクごとに memcpy (ゼロコピー・バッチアップロード)
        uint8_t* dest = static_cast<uint8_t*>(mapped_data);
        for (size_t i = 0; i < storage->get_chunk_count(); ++i) {
            void* src = storage->get_chunk_ptr(i);
            size_t copy_count = std::min((size_t)1024, total_count - (i * 1024));
            memcpy(dest + (i * 1024 * sizeof(Transform)), src, copy_count * sizeof(Transform));
        }
    }

private:
    void recreate_buffer(size_t size) {
        // DX12 Upload Buffer の作成ロジック (簡略化)
        D3D12_HEAP_PROPERTIES heap_props = {};
        heap_props.Type = D3D12_HEAP_TYPE_UPLOAD;

        D3D12_RESOURCE_DESC res_desc = {};
        res_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        res_desc.Width = size;
        res_desc.Height = 1;
        res_desc.DepthOrArraySize = 1;
        res_desc.MipLevels = 1;
        res_desc.SampleDesc.Count = 1;
        res_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        device->CreateCommittedResource(
            &heap_props, D3D12_HEAP_FLAG_NONE, &res_desc,
            D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&gpu_buffer));

        gpu_buffer->Map(0, nullptr, &mapped_data);
        buffer_capacity = size;
    }
};
