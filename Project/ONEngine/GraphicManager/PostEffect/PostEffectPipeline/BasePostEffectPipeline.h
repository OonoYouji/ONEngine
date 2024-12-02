#pragma once

/// directX
#include <d3d12.h>

/// std
#include <memory>

/// engine
#include "GraphicManager/PipelineState/ComputePipelineState.h"
#include "GraphicManager/TextureManager/Texture.h"


/// ===================================================
/// ポストエフェクトの基底クラス
/// ===================================================
class BasePostEffectPipeline {
public:
	
	/// ===================================================
	/// public : methods
	/// ===================================================
	
	virtual ~BasePostEffectPipeline() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// shaderの実行処理
	/// </summary>
	/// <param name="_input">  : 入力resource   </param>
	/// <param name="_output"> : 出力先resource </param>
	virtual void Execution(Texture* _input, Texture* _output) = 0;

protected:
	
	/// ===================================================
	/// private : objects
	/// ===================================================
	
	std::unique_ptr<ComputePipelineState> computePipeline_ = nullptr;
	ShaderBlob                            shaderBlob_;


public:
	
	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// <summary>
	/// pipelineのセッタ
	/// </summary>
	/// <param name="_computePipeline"> : csのpipeline </param>
	void SetPipeline(ComputePipelineState* _computePipeline);

};