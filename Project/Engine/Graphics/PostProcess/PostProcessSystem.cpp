#include "PostProcessSystem.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Graphics/Core/DescriptorHeap.h"
#include "Engine/Graphics/Shader/ShaderManager.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"

namespace Engine::Graphics {

void PostProcessSystem::Initialize(RenderDevice* device, DescriptorHeap* rtvHeap, DescriptorHeap* srvHeap, const Engine::Math::Vector2Int& size) {
    device_ = device;
}

void PostProcessSystem::Shutdown() {
}

void PostProcessSystem::Render(ID3D12GraphicsCommandList* commandList, RenderTexture* inputSource, D3D12_CPU_DESCRIPTOR_HANDLE outputDestination) {
    auto& shaderManager = ShaderManager::GetInstance();
    
    // 1. 出力先を RTV として設定
    commandList->OMSetRenderTargets(1, &outputDestination, FALSE, nullptr);

    // 2. パイプライン設定
    PipelineStateDesc desc;
    desc.rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.dsvFormat = DXGI_FORMAT_UNKNOWN;
    desc.depthEnable = false;
    desc.depthWriteEnable = false;

    auto* pso = shaderManager.GetOrCreatePSO("PostProcess", desc);
    auto* rootSig = shaderManager.GetRootSignature("PostProcess");

    commandList->SetGraphicsRootSignature(rootSig->Get());
    commandList->SetPipelineState(pso->Get());

    // --- 記述子ヒープのセット (D3D12 ERROR #708 の修正) ---
    // 入力ソース (mainColorBuffer) が作成された際に使用された GraphicsEngine のヒープをバインドする
    ID3D12DescriptorHeap* heaps[] = { GraphicsEngine::GetInstance().GetSRVHeap()->GetHeap() };
    commandList->SetDescriptorHeaps(_countof(heaps), heaps);

    // 3. リソースバインド
    commandList->SetGraphicsRootDescriptorTable(rootSig->GetParameterIndex("gMainTexture"), inputSource->GetSRVHandle());

    // 4. 描画
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->DrawInstanced(3, 1, 0, 0); // 全画面三角形
}

} // namespace Engine::Graphics
