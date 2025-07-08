#pragma once

/// std
#include <memory>

/// engine
#include "../../Interface/IRenderingPipeline.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"

#include "Engine/Graphics/Resource/ResourceData/Skinning.h"

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

	SkinMeshRenderingPipeline(class GraphicsResourceCollection* _graphicsResourceCollection);
	~SkinMeshRenderingPipeline() override = default;

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) override;

	void Draw(const std::vector<IEntity*>& _entities, class Camera* _camera, DxCommand* _dxCommand) override;
	
private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	class GraphicsResourceCollection* pGraphicsResourceCollection_ = nullptr;

	std::unique_ptr<ConstantBuffer<Matrix4x4>> transformBuffer_;
	std::unique_ptr<ConstantBuffer<Vector4>> materialBuffer_;
	std::unique_ptr<ConstantBuffer<uint32_t>> textureIdBuffer_;

};

