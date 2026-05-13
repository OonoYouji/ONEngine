#include "GeometryPool.h"
#include "Engine/Asset/Mesh.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Graphics/Resource/GpuBuffer.h"
#include "Engine/Common/Assert.h"
#include "Engine/Common/Console.h"

namespace Engine::Graphics {

GeometryPool* GeometryPool::instance_ = nullptr;

void GeometryPool::Initialize(RenderDevice* device) {
    vertexBuffer_ = std::make_unique<StructuredBuffer>();
    vertexBuffer_->Create(device, sizeof(Asset::Vertex), kMaxVertices);

    boneWeightBuffer_ = std::make_unique<StructuredBuffer>();
    boneWeightBuffer_->Create(device, sizeof(Engine::GeneratedSchema::BoneWeightData), kMaxVertices);
    
    indexBuffer_ = std::make_unique<IndexBuffer>();
    indexBuffer_->Create(device, kMaxIndices);

    currentVertexOffset_ = 0;
    currentIndexOffset_ = 0;

    Engine::Console::Log("GeometryPool: Initialized.");
}

void GeometryPool::Shutdown() {
    vertexBuffer_.reset();
    boneWeightBuffer_.reset();
    indexBuffer_.reset();
}

GeometryPool::Allocation GeometryPool::Allocate(const std::vector<Asset::Vertex>& vertices, const std::vector<uint32_t>& indices, const std::vector<Engine::GeneratedSchema::BoneWeightData>& boneWeights) {
    uint32_t vCount = static_cast<uint32_t>(vertices.size());
    uint32_t iCount = static_cast<uint32_t>(indices.size());

    Assert(currentVertexOffset_ + vCount <= kMaxVertices, "GeometryPool: Vertex buffer overflow!");
    Assert(currentIndexOffset_ + iCount <= kMaxIndices, "GeometryPool: Index buffer overflow!");

    Allocation alloc;
    alloc.vertexOffset = currentVertexOffset_;
    alloc.indexOffset = currentIndexOffset_;

    Engine::Console::Log(std::format("GeometryPool: Allocating Mesh - Vertices: {}, Indices: {}, VOffset: {}, IOffset: {}", vCount, iCount, alloc.vertexOffset, alloc.indexOffset));

    // GPUバッファへアップロード
    vertexBuffer_->Update(vertices.data(), vCount * sizeof(Asset::Vertex), currentVertexOffset_ * sizeof(Asset::Vertex));
    
    if (!boneWeights.empty()) {
        Assert(boneWeights.size() == vertices.size(), "Bone weights size mismatch.");
        boneWeightBuffer_->Update(boneWeights.data(), vCount * sizeof(Engine::GeneratedSchema::BoneWeightData), currentVertexOffset_ * sizeof(Engine::GeneratedSchema::BoneWeightData));
    }

    indexBuffer_->Update(indices.data(), iCount, currentIndexOffset_);

    currentVertexOffset_ += vCount;
    currentIndexOffset_ += iCount;

    return alloc;
}

} // namespace Engine::Graphics
