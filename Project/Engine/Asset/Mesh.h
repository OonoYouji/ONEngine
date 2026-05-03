#pragma once

#include "Engine/Graphics/Resource/GpuBuffer.h"
#include <memory>
#include <vector>
#include "Engine/Core/Math/Math.h"

namespace Engine::Asset {

struct Vertex {
    Engine::Math::Vector3 position;
    Engine::Math::Vector2 uv;
};

///
/// メッシュクラス
///
class Mesh {
public:
    Mesh();
    ~Mesh();

    void Create(Graphics::RenderDevice* device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

    void Draw(ID3D12GraphicsCommandList* commandList);

    Graphics::StructuredBuffer* GetVertexBuffer() { return vertexBuffer_.get(); }
    Graphics::IndexBuffer* GetIndexBuffer() { return indexBuffer_.get(); }

private:
    std::unique_ptr<Graphics::StructuredBuffer> vertexBuffer_;
    std::unique_ptr<Graphics::IndexBuffer> indexBuffer_;
};

} // namespace Engine::Asset
