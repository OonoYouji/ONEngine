#pragma once

#include <d3d12.h>
#include <cstdint>
#include "Engine/Core/Math/Math.h"

namespace Engine::ECS { class AnimationSystem; }

namespace Engine::Graphics {

class GPUCullingManager;

///
/// 描画パスに必要なコンテキスト情報をまとめた構造体
///
struct RenderContext {
    ID3D12GraphicsCommandList* commandList = nullptr;
    D3D12_GPU_VIRTUAL_ADDRESS sceneCBAddress = 0;
    D3D12_GPU_VIRTUAL_ADDRESS pointLightBufferAddress = 0;
    D3D12_GPU_VIRTUAL_ADDRESS lightGridBufferAddress = 0;
    D3D12_GPU_VIRTUAL_ADDRESS lightIndexListBufferAddress = 0;
    D3D12_GPU_VIRTUAL_ADDRESS meshInfoBufferAddress = 0;

    uint32_t frameIndex = 0;
    uint32_t numRenderTargets = 0;
    DXGI_FORMAT rtvFormats[8] = { DXGI_FORMAT_UNKNOWN };

    GPUCullingManager* cullingManager = nullptr;
    Engine::ECS::AnimationSystem* animationSystem = nullptr;

    Engine::Math::Matrix4x4 viewProj;
};

} // namespace Engine::Graphics
