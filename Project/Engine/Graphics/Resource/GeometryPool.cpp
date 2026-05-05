#include "GeometryPool.h"
#include "Engine/Asset/Mesh.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Common/Assert.h"
#include "Engine/Common/Console.h"

namespace Engine::Graphics {

void GeometryPool::Initialize(RenderDevice* device) {
    device_ = device;
    
    vertexBuffer_ = std::make_unique<StructuredBuffer>();
    vertexBuffer_->Create(device, sizeof(Asset::Vertex), kMaxVertices);
    
    indexBuffer_ = std::make_unique<IndexBuffer>();
    indexBuffer_->Create(device, kMaxIndices);

    currentVertexOffset_ = 0;
    currentIndexOffset_ = 0;

    Engine::Console::Log("GeometryPool: Initialized.");
}

void GeometryPool::Shutdown() {
    vertexBuffer_.reset();
    indexBuffer_.reset();
}

GeometryPool::Allocation GeometryPool::Allocate(const std::vector<Asset::Vertex>& vertices, const std::vector<uint32_t>& indices) {
    uint32_t vCount = static_cast<uint32_t>(vertices.size());
    uint32_t iCount = static_cast<uint32_t>(indices.size());

    Assert(currentVertexOffset_ + vCount <= kMaxVertices, "GeometryPool: Vertex buffer overflow!");
    Assert(currentIndexOffset_ + iCount <= kMaxIndices, "GeometryPool: Index buffer overflow!");

    Allocation alloc;
    alloc.vertexOffset = currentVertexOffset_;
    alloc.indexOffset = currentIndexOffset_;

    // GPUバッファへアップロード
    vertexBuffer_->Update(vertices.data(), vCount * sizeof(Asset::Vertex), currentVertexOffset_ * sizeof(Asset::Vertex));
    indexBuffer_->Update(indices.data(), iCount, currentIndexOffset_);

    currentVertexOffset_ += vCount;
    currentIndexOffset_ += iCount;

    return alloc;
}

} // namespace Engine::Graphics
