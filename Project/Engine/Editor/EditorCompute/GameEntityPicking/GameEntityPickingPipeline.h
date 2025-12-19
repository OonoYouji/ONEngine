#pragma once

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"

/// editor
#include "../Interface/IEditorCompute.h"

namespace Editor {

/// ///////////////////////////////////////////////////
/// ゲームエンティティのピッキングを行い、選択状態を管理するパイプラインクラス
/// ///////////////////////////////////////////////////
class GameEntityPickingPipeline : public IEditorCompute {

	enum ROOT_PARAM {
		CBV_PICKING_PARAMS = 0,
		UAV_PICKING,
		SRV_FLAGS_TEXTURE
	};


	struct PickingParams {
		ONEngine::Vector2 mousePosNorm;
	};

	struct Picking {
		int32_t entityId;
	};

public:
	/// =========================================
	/// public : methods
	/// =========================================
	
	GameEntityPickingPipeline();
	~GameEntityPickingPipeline() override;

	void Initialize(ONEngine::ShaderCompiler* _shaderCompiler, ONEngine::DxManager* _dxm) override;
	void Execute(ONEngine::EntityComponentSystem* _ecs, ONEngine::DxCommand* _dxCommand, ONEngine::AssetCollection* _assetCollection) override;

private:
	/// =========================================
	/// private : objects
	/// =========================================

	ONEngine::ConstantBuffer<PickingParams> cbufPickingParams_;
	ONEngine::StructuredBuffer<Picking>     sbufPicking_;

};

} /// namespace Editor