#pragma once

#include "Engine/Graphics/Resource/GpuBuffer.h"
#include <memory>
#include <vector>
#include "Engine/Core/Math/Math.h"

namespace Engine::Asset {

#pragma pack(push, 1)
struct Vertex {
    Engine::Math::Vector4 position; // 16 bytes
    Engine::Math::Vector4 normal;   // 16 bytes
    Engine::Math::Vector2 uv;       // 8 bytes
    float _pad[2];                  // 8 bytes (Total 48 bytes)
};
#pragma pack(pop)

///
/// メッシュクラス
///
class Mesh {
public:
    Mesh();
    ~Mesh();

    void Create(Graphics::RenderDevice* device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

    void Draw(ID3D12GraphicsCommandList* commandList, uint32_t instanceCount = 1);

    uint32_t GetVertexOffset() const { return vertexOffset_; }
    uint32_t GetIndexOffset() const { return indexOffset_; }
    uint32_t GetVertexCount() const { return vertexCount_; }
    uint32_t GetIndexCount() const { return indexCount_; }

    const Engine::Math::Vector3& GetAABBMin() const { return aabbMin_; }
    const Engine::Math::Vector3& GetAABBMax() const { return aabbMax_; }

private:
    uint32_t vertexOffset_ = 0;
    uint32_t indexOffset_ = 0;
    uint32_t vertexCount_ = 0;
    uint32_t indexCount_ = 0;

    Engine::Math::Vector3 aabbMin_;
    Engine::Math::Vector3 aabbMax_;
};

} // namespace Engine::Asset
