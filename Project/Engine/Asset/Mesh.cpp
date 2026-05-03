#include "Mesh.h"
#include "Engine/Graphics/Core/RenderDevice.h"

namespace Engine::Graphics {

Mesh::Mesh() = default;
Mesh::~Mesh() = default;

void Mesh::Create(RenderDevice* device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
    vertexBuffer_ = std::make_unique<StructuredBuffer>();
    vertexBuffer_->Create(device, sizeof(Vertex), static_cast<uint32_t>(vertices.size()), vertices.data());

    indexBuffer_ = std::make_unique<IndexBuffer>();
    indexBuffer_->Create(device, static_cast<uint32_t>(indices.size()), indices.data());
}

void Mesh::Draw(ID3D12GraphicsCommandList* commandList) {
    if (!vertexBuffer_ || !indexBuffer_) return;

    D3D12_INDEX_BUFFER_VIEW ibv = indexBuffer_->GetView();
    commandList->IASetIndexBuffer(&ibv);
    commandList->DrawIndexedInstanced(indexBuffer_->GetCount(), 1, 0, 0, 0);
}

} // namespace Engine::Graphics
