#pragma once

#include "../PostEffectPipeline/BasePostEffectPipeline.h"


/// ===================================================
/// 
/// ===================================================
class Grayscale : BasePostEffectPipeline {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================
	
	Grayscale();
	~Grayscale();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// shaderの実行処理
	/// </summary>
	/// <param name="_input">  : 入力resource   </param>
	/// <param name="_output"> : 出力先resource </param>
	void Execution(ID3D12Resource* _input, ID3D12Resource* _output) override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

};