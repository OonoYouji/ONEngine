#include "Mesh.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Graphics/Resource/GeometryPool.h"

namespace Engine::Asset {

Mesh::Mesh() = default;
Mesh::~Mesh() = default;

void Mesh::Create(Graphics::RenderDevice* device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
    auto alloc = Graphics::GeometryPool::GetInstance().Allocate(vertices, indices);
    
    vertexOffset_ = alloc.vertexOffset;
    indexOffset_ = alloc.indexOffset;
    vertexCount_ = static_cast<uint32_t>(vertices.size());
    indexCount_ = static_cast<uint32_t>(indices.size());
}

void Mesh::Draw(ID3D12GraphicsCommandList* commandList, uint32_t instanceCount) {
    commandList->DrawIndexedInstanced(
        indexCount_, 
        instanceCount, 
        indexOffset_, 
        0, // BaseVertexLocation を 0 に固定 (シェーダー側で解決)
        0
    );
}

} // namespace Engine::Asset
