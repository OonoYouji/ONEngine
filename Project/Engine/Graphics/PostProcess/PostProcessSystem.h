#pragma once

#include <d3d12.h>
#include <memory>
#include <vector>
#include "Engine/Graphics/Core/RenderContext.h"
#include "Engine/Core/Math/Math.h"

namespace Engine::Graphics {

class RenderDevice;
class DescriptorHeap;
class RenderTexture;
class ConstantBuffer;

///
/// ポストプロセス（Bloom, Blur, ToneMappingなど）を管理するクラス
///
class PostProcessSystem {
public:
    static PostProcessSystem& GetInstance() {
        return *instance_;
    }

    static void CreateInstance() {
        if (!instance_) instance_ = new PostProcessSystem();
    }

    static void DestroyInstance() {
        delete instance_;
        instance_ = nullptr;
    }

    void Initialize(RenderDevice* device, DescriptorHeap* rtvHeap, DescriptorHeap* srvHeap, const Engine::Math::Vector2Int& size);
    void Shutdown();

    void Render(ID3D12GraphicsCommandList* commandList, RenderTexture* inputSource, D3D12_CPU_DESCRIPTOR_HANDLE outputDestination);

private:
    PostProcessSystem() = default;
    ~PostProcessSystem() = default;

    static PostProcessSystem* instance_;

    RenderDevice* device_ = nullptr;
    Engine::Math::Vector2Int size_;

    static constexpr uint32_t kBloomLevels = 4;
    std::unique_ptr<RenderTexture> downsampleBuffers_[kBloomLevels];
    std::unique_ptr<RenderTexture> upsampleBuffers_[kBloomLevels];
    
    std::unique_ptr<ConstantBuffer> blurCB_;
    std::unique_ptr<ConstantBuffer> postProcessCB_;

    struct BlurParams {
        Engine::Math::Vector2 direction;
        float textureSize;
        float _pad;
    };

    struct PostProcessParams {
        float threshold;
        float intensity;
        float exposure;
        float _pad;
        Engine::Math::Vector4 outlineColor;
        float outlineThreshold;
        float outlineWidth;
        float _pad2[2];
    };
};

} // namespace Engine::Graphics
