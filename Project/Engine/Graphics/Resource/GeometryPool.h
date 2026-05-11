#pragma once

#include <d3d12.h>
#include <vector>
#include <memory>
#include "Engine/Graphics/Utils/ComPtr.h"
#include "Schema/Schema.h"

namespace Engine::Asset { struct Vertex; }

namespace Engine::Graphics {

class RenderDevice;
class StructuredBuffer;
class IndexBuffer;

///
/// 全てのメッシュ頂点・インデックス・ボーンウェイトを一括管理するプール
///
class GeometryPool {
public:
    static constexpr uint32_t kMaxVertices = 1000000;
    static constexpr uint32_t kMaxIndices = 3000000;

    static GeometryPool& GetInstance() {
        return *instance_;
    }

    static void CreateInstance() {
        if (!instance_) instance_ = new GeometryPool();
    }

    static void DestroyInstance() {
        delete instance_;
        instance_ = nullptr;
    }

    void Initialize(RenderDevice* device);
    void Shutdown();

    struct Allocation {
        uint32_t vertexOffset;
        uint32_t indexOffset;
    };

    Allocation Allocate(const std::vector<Engine::Asset::Vertex>& vertices, const std::vector<uint32_t>& indices, const std::vector<Engine::GeneratedSchema::BoneWeightData>& weights);

    StructuredBuffer* GetVertexBuffer() const { return vertexBuffer_.get(); }
    IndexBuffer*      GetIndexBuffer()  const { return indexBuffer_.get(); }
    StructuredBuffer* GetBoneWeightBuffer() const { return boneWeightBuffer_.get(); }

private:
    GeometryPool() = default;
    ~GeometryPool() = default;

    static GeometryPool* instance_;

    std::unique_ptr<StructuredBuffer> vertexBuffer_;
    std::unique_ptr<IndexBuffer>      indexBuffer_;
    std::unique_ptr<StructuredBuffer> boneWeightBuffer_;

    uint32_t currentVertexOffset_ = 0;
    uint32_t currentIndexOffset_ = 0;
};

} // namespace Engine::Graphics
