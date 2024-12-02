#include "PostEffectPipelineManager.h"

#include "Debugger/Assertion.h"


void PostEffectPipelineManager::AddPipeline(
	const std::string _key, std::unique_ptr<BasePostEffectPipeline> _pipeline) {

	pipelineMap_[_key] = std::move(_pipeline);
}

BasePostEffectPipeline* PostEffectPipelineManager::GetPostEffectPipeline(const std::string& _key) {
	if(auto itr = pipelineMap_.find(_key); itr == pipelineMap_.end()) {
		Assert(false, "There was no pipeline.");
	}
	return pipelineMap_.at(_key).get();
}
