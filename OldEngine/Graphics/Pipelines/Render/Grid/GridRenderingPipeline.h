#pragma once

/// engine
#include "../../Interface/IRenderingPipeline.h"
#include "Engine/Core/Utility/Math/Vector4.h"
#include "Engine/Graphics/Buffer/VertexBuffer.h"

namespace ONEngine {

/// 前方宣言
class DxManager;
class DxCommand;
class ECSGroup;
class CameraComponent;
class ShaderCompiler;

/// /////////////////////////////////////////////////
/// エディタ上で描画するグリッドの表示のためのパイプライン
/// /////////////////////////////////////////////////
class GridRenderingPipeline : public IRenderingPipeline {

	enum ROOT_PARAM {
		CBV_VIEW_PROJECTION,
		CBV_CAMERA_POSITION
	};

public:

	GridRenderingPipeline();
	~GridRenderingPipeline();

	void Initialize(ShaderCompiler* sc, DxManager* dxm) override;
	void Draw(ECSGroup* ecs, CameraComponent* camera, DxCommand* dxCommand) override;

private:

	VertexBuffer<Vector4> vertexBuffer;

};

} /// namespace ONEngine
