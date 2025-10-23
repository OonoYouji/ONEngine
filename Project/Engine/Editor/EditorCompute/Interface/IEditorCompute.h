#pragma once

/// std
#include <memory>

/// engine
#include "Engine/Graphics/Shader/ComputePipeline.h"
#include "Engine/Graphics/Shader/ShaderCompiler.h"

/// /////////////////////////////////////////////////
/// コンピュートシェーダーを利用したエディターのパイプライン
/// /////////////////////////////////////////////////
class IEditorCompute {
public:
	/// =========================================
	/// public : methods
	/// =========================================

	IEditorCompute() = default;
	virtual ~IEditorCompute() = default;

	virtual void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) = 0;
	virtual void Execute(class EntityComponentSystem* _ecs, class DxCommand* _dxCommand, class AssetCollection* _assetCollection) = 0;


protected:
	/// =========================================
	/// protected : objects
	/// =========================================

	std::unique_ptr<ComputePipeline> pipeline_;

};

