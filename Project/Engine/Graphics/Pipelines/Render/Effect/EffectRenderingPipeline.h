#pragma once

/// std
#include <cmath>

/// engine
#include "../../Interface/IRenderingPipeline.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"
#include "Engine/Core/Utility/Math/Matrix4x4.h"
#include "Engine/Core/Utility/Math/Vector4.h"

/// //////////////////////////////////////////////////
/// エフェクトの描画パイプライン
/// //////////////////////////////////////////////////
class EffectRenderingPipeline : public IRenderingPipeline {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	EffectRenderingPipeline(class GraphicsResourceCollection* _resourceCollection);
	~EffectRenderingPipeline();

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) override;

	void Draw(DxCommand* _dxCommand, class EntityComponentSystem* _pEntityComponentSystem, class Camera* _camera) override;

private:
	/// ====================================================
	/// private : objects
	/// ====================================================

	class GraphicsResourceCollection* pResourceCollection_ = nullptr;

	const size_t kMaxRenderingMeshCount_ = std::pow(2, 20);
	std::unique_ptr<StructuredBuffer<Matrix4x4>> transformBuffer_;
	std::unique_ptr<StructuredBuffer<Vector4>>   materialBuffer;
	std::unique_ptr<StructuredBuffer<uint32_t>>  textureIdBuffer_;

	size_t transformIndex_;
	uint32_t instanceIndex_;

};

