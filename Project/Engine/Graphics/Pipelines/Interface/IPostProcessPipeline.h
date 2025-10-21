#pragma once

/// std
#include <memory>
#include <string>

/// engine
#include "Engine/Graphics/Shader/ComputePipeline.h"
#include "Engine/Graphics/Shader/ShaderCompiler.h"

/// ///////////////////////////////////////////////////
/// PostProcessのinterfaceクラス
/// ///////////////////////////////////////////////////
class IPostProcessPipeline {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	virtual ~IPostProcessPipeline() = default;

	/// @brief pipelineの初期化を行う
	/// @param _shaderCompiler ShaderCompilerへのポインタ
	/// @param _dxManager DxManagerへのポインタ
	virtual void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) = 0;

	/// @brief post processの実行
	virtual void Execute(
		const std::string& _textureName,
		class DxCommand* _dxCommand, 
		class AssetCollection* _resourceCollection,
		class EntityComponentSystem* _pEntityComponentSystem
	) = 0;

protected:
	/// ===================================================
	/// protected : objects
	/// ===================================================

	std::unique_ptr<ComputePipeline> pipeline_;

};


/// ===================================================
/// 部分的なpost processとスクリーンに適用するpost processのinterfaceを統一
/// ===================================================
using PerObjectPostProcess = IPostProcessPipeline;
using ScreenPostProcess = IPostProcessPipeline;


void CopyResource(ID3D12Resource* _src, ID3D12Resource* _dst, ID3D12GraphicsCommandList6* _cmdList);