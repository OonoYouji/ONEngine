#pragma once

#include "../PostEffectPipeline/BasePostEffectPipeline.h"

/// ===================================================
/// 画面のぼかし
/// ===================================================
class GaussianFilter : public BasePostEffectPipeline {
public:

	/// ===================================================
	/// 
	/// ===================================================

	GaussianFilter();
	~GaussianFilter();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// shaderの実行処理
	/// </summary>
	/// <param name="_input">  : 入力resource   </param>
	/// <param name="_output"> : 出力先resource </param>
	void Execution(Texture* _input, Texture* _output) override;

private:

	/// ===================================================
	/// 
	/// ===================================================


};