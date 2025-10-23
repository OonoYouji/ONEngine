#pragma once

/// std
#include <array>

/// engine 
#include "Engine/Core/Utility/Math/Vector2.h"
#include "Engine/Graphics/Pipelines/Interface/IPostProcessPipeline.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"

/// ///////////////////////////////////////////////////
/// 地形エディタのブラシを表示するポストプロセス
/// ///////////////////////////////////////////////////
class PostProcessTerrainBrush : public PerObjectPostProcess {

	enum ROOT_PARAM {
		CBV_BRUSH,
		SRV_COLOR,
		SRV_POSITION,
		SRV_FLAGS,
		UAV_RESULT
	};


	struct Brush {
		Vector2 position;
		float radius;
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	PostProcessTerrainBrush();
	~PostProcessTerrainBrush();

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) override;

	void Execute(
		const std::string& _textureName,
		class DxCommand* _dxCommand,
		class AssetCollection* _assetCollection,
		class EntityComponentSystem* _entityComponentSystem
	) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::array<size_t, 4> textureIndices_;
	ConstantBuffer<Brush> brushBuffer_;
};