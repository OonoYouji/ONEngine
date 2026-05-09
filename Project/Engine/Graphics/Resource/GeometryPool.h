#pragma once

#include <memory>
#include <vector>
#include <cstdint>
#include "GpuBuffer.h"
#include "Schema/Schema.h"

namespace Engine::Asset {
    struct Vertex;
}

namespace Engine::Graphics {

class RenderDevice;

///
/// 全てのジオメトリ（頂点・インデックス）を一括管理するプール
///
class GeometryPool {
public:
    static GeometryPool& GetInstance() {
        static GeometryPool instance;
        return instance;
    }

    void Initialize(RenderDevice* device);
    void Shutdown();

    /// @brief データの書き込みと領域の確保
    /// @return 確保されたオフセット情報
    struct Allocation {
        uint32_t vertexOffset;
        uint32_t indexOffset;
    };
    Allocation Allocate(const std::vector<Asset::Vertex>& vertices, const std::vector<uint32_t>& indices, const std::vector<Engine::GeneratedSchema::BoneWeightData>& boneWeights = {});

    StructuredBuffer* GetVertexBuffer() { return vertexBuffer_.get(); }
    StructuredBuffer* GetBoneWeightBuffer() { return boneWeightBuffer_.get(); }
    IndexBuffer* GetIndexBuffer() { return indexBuffer_.get(); }

private:
    GeometryPool() = default;
    ~GeometryPool() = default;

private:
    RenderDevice* device_ = nullptr;
    std::unique_ptr<StructuredBuffer> vertexBuffer_;
    std::unique_ptr<StructuredBuffer> boneWeightBuffer_;
    std::unique_ptr<IndexBuffer> indexBuffer_;

    uint32_t currentVertexOffset_ = 0;
    uint32_t currentIndexOffset_ = 0;

    // とりあえず固定の最大サイズ (必要に応じて拡張またはアロケータを実装)
    static constexpr uint32_t kMaxVertices = 1000000;
    static constexpr uint32_t kMaxIndices = 2000000;
};

} // namespace Engine::Graphics
