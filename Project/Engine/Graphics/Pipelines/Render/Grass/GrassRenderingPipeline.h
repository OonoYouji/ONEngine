#pragma once

/// engine
#include "../../Interface/IRenderingPipeline.h"

#include "Engine/Core/Utility/Utility.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"
#include "Engine/Graphics/Buffer/ByteAddressBuffer.h"

#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Grass/GrassField.h"

/// /////////////////////////////////////////////////
/// 草を描画するパイプライン
/// /////////////////////////////////////////////////
class GrassRenderingPipeline : public IRenderingPipeline {

	enum ROOT_PARAM {
		CBV_VIEW_PROJECTION,
		ROOT_PARAM_CONSTANTS,
		CBV_MATERIAL,
		ROOT_PARAM_BLADES,
		ROOT_PARAM_TIME,
		SRV_TEXTURES,
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	GrassRenderingPipeline(class GraphicsResourceCollection* _grc);
	~GrassRenderingPipeline();

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) override;
	void Draw(class ECSGroup* _ecs, const std::vector<class GameEntity*>& _entities, class CameraComponent* _camera, DxCommand* _dxCommand) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	class GraphicsResourceCollection* pGrc_ = nullptr;
};

