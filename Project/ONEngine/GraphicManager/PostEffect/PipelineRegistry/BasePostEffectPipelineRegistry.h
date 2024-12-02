#pragma once

/// std
#include <memory>
#include <unordered_map>
#include <string>


/// engine
#include "GraphicManager/PostEffect/PostEffectPipeline/BasePostEffectPipeline.h"


/// ===================================================
/// ポストエフェクトのパイプラインを登録、管理するクラスの基底クラス
/// ===================================================
class BasePostEffectPipelineRegistry {
public:
	
	/// ===================================================
	/// public : methods
	/// ===================================================

	virtual ~BasePostEffectPipelineRegistry() {}

	/// <summary>
	/// pipeline mapに登録する
	/// </summary>
	/// <param name="_postEffectName"> : mapのkey   </param>
	/// <param name="_pipeline">       : mapのvalue </param>
	void Register(const std::string& _postEffectName, std::unique_ptr<BasePostEffectPipeline> _pipeline);


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unordered_map<std::string, std::unique_ptr<BasePostEffectPipeline>> pipelineMap_;


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// <summary>
	/// pipeline pointer のゲッタ
	/// </summary>
	/// <param name="_pipelineName"> : mapのkey         </param>
	/// <returns>                    : pipeline pointer </returns>
	BasePostEffectPipeline* GetPipeline(const std::string& _pipelineName);

};