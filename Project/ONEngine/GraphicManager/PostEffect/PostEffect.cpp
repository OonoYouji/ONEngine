#include "PostEffect.h"

/// engine
#include "GraphicManager/SceneLayer/SceneLayer.h"


PostEffect::PostEffect() {}
PostEffect::~PostEffect() {}

void PostEffect::Initialize() {}

void PostEffect::Execution() {
	size_t input = 0;

	for(auto& pipeline : postEffectPipelines_) {
		pipeline->Execution(
			intermediateTextures_[input],
			intermediateTextures_[1 - input]
		);

		/// inputを反転させる
		input = input ? 0 : 1;
	}
}

void PostEffect::AddPipeline(BasePostEffectPipeline* _postEffectPipeline) {
	postEffectPipelines_.push_back(_postEffectPipeline);
}

void PostEffect::SetLayerRenderTexture(RenderTexture* _renderTexture) {
	pLayerRenderTexture_ = _renderTexture;
}
