#include "BasePostEffectPipelineRegistry.h"



void BasePostEffectPipelineRegistry::Register(const std::string& _postEffectName, std::unique_ptr<BasePostEffectPipeline> _pipeline) {
	pipelineMap_[_postEffectName] = std::move(_pipeline);
}

BasePostEffectPipeline* BasePostEffectPipelineRegistry::GetPipeline(const std::string& _pipelineName) {
	return pipelineMap_.at(_pipelineName).get();
}
