#pragma once

/// std
#include <string>
#include <memory>
#include <unordered_map>

/// engine
#include "BasePostEffectPipeline.h"


/// ===================================================
/// ポストエフェクトのパイプラインを管理するクラス
/// ===================================================
class PostEffectPipelineManager {
	PostEffectPipelineManager() = default;
	~PostEffectPipelineManager() = default;
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	static PostEffectPipelineManager* GetInstance() {
		static PostEffectPipelineManager instance;
		return &instance;
	}

	/// <summary>
	/// パイプラインの追加
	/// </summary>
	/// <param name="_key">       : 追加するパイプラインの名前           </param>
	/// <param name="_pipeline">  : 追加するポストエフェクトのパイプライン </param>
	void AddPipeline(const std::string _key, std::unique_ptr<BasePostEffectPipeline> _pipeline);

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
	/// ポストエフェクトのパイプラインのゲッタ
	/// </summary>
	/// <param name="_key"> : mapのkey </param>
	/// <returns>           : pipeline </returns>
	BasePostEffectPipeline* GetPostEffectPipeline(const std::string& _key);




private:
	PostEffectPipelineManager(const PostEffectPipelineManager&)            = delete;
	PostEffectPipelineManager(PostEffectPipelineManager&&)                 = delete;
	PostEffectPipelineManager& operatpr (const PostEffectPipelineManager&) = delete;
	PostEffectPipelineManager& operatpr (PostEffectPipelineManager&&)      = delete;
};