#include "Mesh.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Graphics/Resource/GeometryPool.h"
#include "Engine/Common/Console.h"

namespace Engine::Asset {

Mesh::Mesh() = default;
Mesh::~Mesh() = default;

void Mesh::Create(Graphics::RenderDevice* device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
    auto alloc = Graphics::GeometryPool::GetInstance().Allocate(vertices, indices, boneWeights_);
    
    vertexOffset_ = alloc.vertexOffset;
    indexOffset_ = alloc.indexOffset;
    vertexCount_ = static_cast<uint32_t>(vertices.size());
    indexCount_ = static_cast<uint32_t>(indices.size());

    // AABBの計算
    aabbMin_ = { FLT_MAX, FLT_MAX, FLT_MAX };
    aabbMax_ = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

    for (const auto& v : vertices) {
        aabbMin_.x = (std::min)(aabbMin_.x, v.position.x);
        aabbMin_.y = (std::min)(aabbMin_.y, v.position.y);
        aabbMin_.z = (std::min)(aabbMin_.z, v.position.z);

        aabbMax_.x = (std::max)(aabbMax_.x, v.position.x);
        aabbMax_.y = (std::max)(aabbMax_.y, v.position.y);
        aabbMax_.z = (std::max)(aabbMax_.z, v.position.z);
    }
}

void Mesh::Draw(ID3D12GraphicsCommandList* commandList, uint32_t instanceCount) {
    commandList->DrawIndexedInstanced(
        indexCount_, 
        instanceCount, 
        indexOffset_, 
        0, // vertexOffset はシェーダー側で解決するようになったため 0 固定
        0
    );

    static int drawCount = 0;
    if (drawCount++ % 100 == 0) {
        Engine::Console::Log(std::format("Mesh: Draw Call issued (Indices: {}, Instances: {})", indexCount_, instanceCount));
    }
}

} // namespace Engine::Asset
