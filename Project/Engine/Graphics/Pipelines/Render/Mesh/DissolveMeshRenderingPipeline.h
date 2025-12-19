#pragma once

/// engine
#include "../../Interface/IRenderingPipeline.h"

namespace ONEngine {

class AssetCollection;

/// ///////////////////////////////////////////////////
/// DissolveMeshRendererのデータを使い描画を行うパイプライン
/// ///////////////////////////////////////////////////
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

};

} /// namespace ONEngine