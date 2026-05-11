#include "DebugRenderer.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Graphics/Resource/GpuBuffer.h"
#include "Engine/Graphics/Shader/ShaderManager.h"
#include <d3dx12.h>

namespace Engine::Graphics {

DebugRenderer* DebugRenderer::instance_ = nullptr;

void DebugRenderer::Initialize(RenderDevice* device) {
    vertexBuffer_ = std::make_unique<StructuredBuffer>();
    vertexBuffer_->Create(device, sizeof(Vertex), kMaxVertices);
}

void DebugRenderer::Shutdown() {
    vertexBuffer_.reset();
}

void DebugRenderer::DrawLine(const Engine::Math::Vector3& start, const Engine::Math::Vector3& end, const Engine::Math::Vector4& color) {
    if (vertices_.size() + 2 > kMaxVertices) return;
    vertices_.push_back({ start, color });
    vertices_.push_back({ end, color });
}

void DebugRenderer::DrawBox(const Engine::Math::Vector3& min, const Engine::Math::Vector3& max, const Engine::Math::Vector4& color) {
    DrawLine({ min.x, min.y, min.z }, { max.x, min.y, min.z }, color);
    DrawLine({ max.x, min.y, min.z }, { max.x, max.y, min.z }, color);
    DrawLine({ max.x, max.y, min.z }, { min.x, max.y, min.z }, color);
    DrawLine({ min.x, max.y, min.z }, { min.x, min.y, min.z }, color);

    DrawLine({ min.x, min.y, max.z }, { max.x, min.y, max.z }, color);
    DrawLine({ max.x, min.y, max.z }, { max.x, max.y, max.z }, color);
    DrawLine({ max.x, max.y, max.z }, { min.x, max.y, max.z }, color);
    DrawLine({ min.x, max.y, max.z }, { min.x, min.y, max.z }, color);

    DrawLine({ min.x, min.y, min.z }, { min.x, min.y, max.z }, color);
    DrawLine({ max.x, min.y, min.z }, { max.x, min.y, max.z }, color);
    DrawLine({ max.x, max.y, min.z }, { max.x, max.y, max.z }, color);
    DrawLine({ min.x, max.y, min.z }, { min.x, max.y, max.z }, color);
}

void DebugRenderer::DrawSphere(const Engine::Math::Vector3& center, float radius, const Engine::Math::Vector4& color) {
    // 簡易実装
    DrawLine({ center.x - radius, center.y, center.z }, { center.x + radius, center.y, center.z }, color);
    DrawLine({ center.x, center.y - radius, center.z }, { center.x, center.y + radius, center.z }, color);
    DrawLine({ center.x, center.y, center.z - radius }, { center.x, center.y, center.z + radius }, color);
}

void DebugRenderer::Render(const RenderContext& context) {
    if (vertices_.empty()) return;

    vertexBuffer_->Update(vertices_.data(), (uint32_t)(vertices_.size() * sizeof(Vertex)));

    auto& sm = ShaderManager::GetInstance();
    auto* pso = sm.GetComputePSO("DebugLine"); // 実際には Graphics PSO だが互換性のために一旦
    auto* rootSig = sm.GetRootSignature("DebugLine");

    if (!pso || !rootSig) return;

    context.commandList->SetGraphicsRootSignature(rootSig->Get());
    context.commandList->SetPipelineState(pso);
    context.commandList->SetGraphicsRootConstantBufferView(rootSig->GetParameterIndex("gSceneData"), context.sceneCBAddress);
    context.commandList->SetGraphicsRootShaderResourceView(rootSig->GetParameterIndex("gVertices"), vertexBuffer_->GetResource()->GetGPUVirtualAddress());

    context.commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
    context.commandList->DrawInstanced(static_cast<UINT>(vertices_.size()), 1, 0, 0);
}

void DebugRenderer::Clear() {
    vertices_.clear();
}

} // namespace Engine::Graphics
