#pragma once

/// engine
#include "../../Interface/IRenderingPipeline.h"

/// engine
#include "../../Interface/IRenderingPipeline.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"
#include "Engine/Graphics/Buffer/Data/GPUMaterial.h"
#include "Engine/Core/Utility/Math/Matrix4x4.h"

namespace ONEngine {

class AssetCollection;

/// ///////////////////////////////////////////////////
/// DissolveMeshRendererのデータを使い描画を行うパイプライン
class DissolveMeshRenderingPipeline : public IRenderingPipeline {

	enum ROOT_PARAM {
		CBV_VIEW_PROJECTION,
		SRV_TRANSFORM,
		SRV_MATERIAL,
		SRV_DISSOLVE_PARAMS,
		SRV_TEXTURE_ID,
		SRV_TEXTURE,
		CBV_INSTANCE_OFFSET,
	};

	struct GPUDissolveParams {
		uint32_t textureId;
		uint32_t dissolveCompare;
		float threshold;
	};


public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	DissolveMeshRenderingPipeline(AssetCollection* _ac);
	~DissolveMeshRenderingPipeline() override = default;

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxm) override;
	void Draw(class ECSGroup* _ecsGroup, class CameraComponent* _camera, DxCommand* _dxCommand) override;


private:
	/// ==================================================
	/// private : objects
	/// ==================================================

	AssetCollection* pAssetCollection_ = nullptr;

	const uint32_t kMaxRenderingMeshCount_ = 1024; ///< 最大描画メッシュ数

	StructuredBuffer<Matrix4x4> sbufTransforms_;
	StructuredBuffer<GPUMaterial> sbufMaterials_;
	StructuredBuffer<uint32_t> sbufTextureIds_;
	StructuredBuffer<GPUDissolveParams> sbufDissolveParams_;
};

} /// namespace ONEngine