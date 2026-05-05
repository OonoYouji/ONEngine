#include "DebugRenderer.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Graphics/Shader/ShaderManager.h"
#include <d3dx12.h>

namespace Engine::Graphics {

void DebugRenderer::Initialize(RenderDevice* device) {
    device_ = device;
    vertexBuffer_ = std::make_unique<StructuredBuffer>();
    vertexBuffer_->Create(device, sizeof(Vertex), kMaxVertices);
}

void DebugRenderer::Shutdown() {
    vertexBuffer_.reset();
}

void DebugRenderer::DrawLine(const Engine::Math::Vector3& start, const Engine::Math::Vector3& end, const Engine::Math::Vector4& color) {
    if (vertices_.size() + 2 > kMaxVertices) return;
    vertices_.push_back({start, color});
    vertices_.push_back({end, color});
}

void DebugRenderer::DrawBox(const Engine::Math::Vector3& center, const Engine::Math::Vector3& size, const Engine::Math::Vector4& color) {
    Engine::Math::Vector3 min = {center.x - size.x * 0.5f, center.y - size.y * 0.5f, center.z - size.z * 0.5f};
    Engine::Math::Vector3 max = {center.x + size.x * 0.5f, center.y + size.y * 0.5f, center.z + size.z * 0.5f};

    // Bottom
    DrawLine({min.x, min.y, min.z}, {max.x, min.y, min.z}, color);
    DrawLine({max.x, min.y, min.z}, {max.x, min.y, max.z}, color);
    DrawLine({max.x, min.y, max.z}, {min.x, min.y, max.z}, color);
    DrawLine({min.x, min.y, max.z}, {min.x, min.y, min.z}, color);

    // Top
    DrawLine({min.x, max.y, min.z}, {max.x, max.y, min.z}, color);
    DrawLine({max.x, max.y, min.z}, {max.x, max.y, max.z}, color);
    DrawLine({max.x, max.y, max.z}, {min.x, max.y, max.z}, color);
    DrawLine({min.x, max.y, max.z}, {min.x, max.y, min.z}, color);

    // Vertical
    DrawLine({min.x, min.y, min.z}, {min.x, max.y, min.z}, color);
    DrawLine({max.x, min.y, min.z}, {max.x, max.y, min.z}, color);
    DrawLine({max.x, min.y, max.z}, {max.x, max.y, max.z}, color);
    DrawLine({min.x, min.y, max.z}, {min.x, max.y, max.z}, color);
}

void DebugRenderer::Render(ID3D12GraphicsCommandList* commandList, D3D12_GPU_VIRTUAL_ADDRESS sceneCBAddress, DXGI_FORMAT rtvFormat) {
    if (vertices_.empty()) return;

    // バッファ転送
    vertexBuffer_->Update(vertices_.data(), static_cast<uint32_t>(vertices_.size() * sizeof(Vertex)));

    auto& shaderManager = ShaderManager::GetInstance();
    
    PipelineStateDesc desc;
    desc.rtvFormat = rtvFormat; // 追加
    desc.primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    desc.depthWriteEnable = false; 
    
    auto* pso = shaderManager.GetOrCreatePSO("DebugLine", desc);
    auto* rootSig = shaderManager.GetRootSignature("DebugLine");

    commandList->SetGraphicsRootSignature(rootSig->Get());
    commandList->SetPipelineState(pso->Get());

    auto sceneIdx = rootSig->GetParameterIndex("gSceneData");
    if (sceneIdx != RootSignature::kInvalidIndex) {
        commandList->SetGraphicsRootConstantBufferView(sceneIdx, sceneCBAddress);
    }

    auto vertIdx = rootSig->GetParameterIndex("gVertices");
    if (vertIdx != RootSignature::kInvalidIndex) {
        commandList->SetGraphicsRootShaderResourceView(vertIdx, vertexBuffer_->GetResource()->GetGPUVirtualAddress());
    }

    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
    commandList->DrawInstanced(static_cast<UINT>(vertices_.size()), 1, 0, 0);
}

void DebugRenderer::Clear() {
    vertices_.clear();
}

} // namespace Engine::Graphics
