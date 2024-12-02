#include "BasePostEffectPipeline.h"

void BasePostEffectPipeline::SetPipeline(ComputePipelineState* _computePipeline) {
	computePipeline_.reset(_computePipeline);
}
