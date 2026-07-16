#pragma once

#include <memory>
#include <unordered_map>
#include "../../Interface/IRenderingPipeline.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/ECS/Component/Components/ComputeComponents/ParticleSystem/ParticleSystem.h"
#include "Engine/Core/Utility/Math/Matrix4x4.h"
#include "Engine/Core/DirectX12/Command/DxCommand.h"
#include "Engine/Core/DirectX12/DescriptorHeap/DxSRVHeap.h"

namespace ONEngine {
class ShaderCompiler;
class DxManager;
class ECSGroup;
class CameraComponent;

namespace Asset {
class AssetCollection;
}

class ParticleSystemRenderingPipeline : public IRenderingPipeline {
    struct CameraData {
        Matrix4x4 billboardMatrix;
        Vector3 cameraPosition;
        float padding;
    };

    struct alignas(16) PerSystemData {
        Matrix4x4 emitterWorldMatrix;
        uint32_t renderMode;
        uint32_t renderAlignment;
        float speedScale;
        float lengthScale;
        uint32_t instanceOffset;
        uint32_t padding[3];
    };

    enum ROOT_PARAM {
        CBV_VIEW_PROJECTION,
        CBV_CAMERA_DATA,
        ROOT_PER_SYSTEM,
        SRV_PARTICLES,
        SRV_MATERIALS,
        SRV_TEXTURE_IDS,
        SRV_TEXTURES
    };

public:
    ParticleSystemRenderingPipeline(Asset::AssetCollection* _assetCollection);
    ~ParticleSystemRenderingPipeline() override;

    void Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) override;
    void Draw(ECSGroup* _ecs, CameraComponent* _camera, DxCommand* _dxCommand) override;

private:
    Asset::AssetCollection* pAssetCollection_ = nullptr;
    DxManager* pDxManager_ = nullptr;
    
    ConstantBuffer<CameraData> cameraDataBuffer_;

    const size_t kMaxParticlesTotal_ = size_t(std::pow(2, 20)); // Up to 1M particles total per frame
    StructuredBuffer<Particle> particleBuffer_;
    StructuredBuffer<Vector4> materialBuffer_;
    StructuredBuffer<uint32_t> textureIdBuffer_;

    std::unordered_map<size_t, std::unique_ptr<GraphicsPipeline>> pipelines_;
};

}
