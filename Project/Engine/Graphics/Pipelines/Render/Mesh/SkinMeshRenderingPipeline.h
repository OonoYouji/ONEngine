#pragma once

/// std
#include <memory>

/// engine
#include "../../Interface/IRenderingPipeline.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"

#include "Engine/Asset/Assets/Mesh/Skinning.h"
#include "Engine/Graphics/Buffer/Data/GPUMaterial.h"

/// //////////////////////////////////////////////////////
/// スキンアニメーションの描画パイプライン
/// //////////////////////////////////////////////////////
class SkinMeshRenderingPipeline : public IRenderingPipeline {

	enum {
		ViewProjectionCBV = 0, ///< ViewProjectionのCBV
		TransformCBV,          ///< TransformのCBV
		MaterialCBV,          ///< MaterialのCBV
		TextureIdCBV,         ///< TextureIdのCBV
		WellForGPUSRV,           ///< WellForGPUのSRV
		TextureSRV,           ///< TextureのSRV
	};

public:
	/// ====================================================
	/// public : methods
	/// ====================================================

	SkinMeshRenderingPipeline(class AssetCollection* _graphicsResourceCollection);
	~SkinMeshRenderingPipeline() override = default;

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) override;

	void Draw(class ECSGroup* _ecs, const std::vector<GameEntity*>& _entities, class CameraComponent* _camera, DxCommand* _dxCommand) override;
	
private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	class AssetCollection* pGraphicsResourceCollection_ = nullptr;

	std::unique_ptr<ConstantBuffer<Matrix4x4>> transformBuffer_;
	std::unique_ptr<ConstantBuffer<GPUMaterial>> materialBuffer_;
	std::unique_ptr<ConstantBuffer<uint32_t>> textureIdBuffer_;

};

