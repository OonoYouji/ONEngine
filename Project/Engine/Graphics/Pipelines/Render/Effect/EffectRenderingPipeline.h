#pragma once

/// std
#include <cmath>
#include <unordered_map>
#include <memory>

/// engine
#include "../../Interface/IRenderingPipeline.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"
#include "Engine/Core/Utility/Math/Matrix4x4.h"
#include "Engine/Core/Utility/Math/Vector4.h"

namespace ONEngine {
class ShaderCompiler;
class DxManager;
class ECSGroup;
class CameraComponent;
}

namespace ONEngine::Asset {
class AssetCollection;
}


/// //////////////////////////////////////////////////
/// エフェクトの描画パイプライン
/// //////////////////////////////////////////////////
namespace ONEngine {

class EffectRenderingPipeline : public IRenderingPipeline {


	/// @brief EffectShaderで使用しているRootParameter
	enum ROOT_PARAM {
		CBV_VIEW_PROJECTION,
		SRV_MATERIALS,
		SRV_TEXTURE_IDS,
		SRV_TEXTURES,
		SRV_TRANSFORMS,
		C32BIT_CONSTANT,
	};


public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	EffectRenderingPipeline(Asset::AssetCollection* _assetCollection);
	~EffectRenderingPipeline();

	void Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) override;

	void Draw(ECSGroup* _ecs, CameraComponent* _camera, DxCommand* _dxCommand) override;


private:
	/// ====================================================
	/// private : objects
	/// ====================================================

	Asset::AssetCollection* pAssetCollection_ = nullptr;

	const size_t kMaxRenderingMeshCount_ = size_t(std::pow(2, 20));
	StructuredBuffer<Matrix4x4> transformBuffer_;
	StructuredBuffer<Vector4>   materialBuffer_;
	StructuredBuffer<uint32_t>  textureIdBuffer_;

	size_t transformIndex_;
	uint32_t instanceIndex_;

	std::unordered_map<size_t, std::unique_ptr<GraphicsPipeline>> pipelines_;

};


} /// ONEngine
