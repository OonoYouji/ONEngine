#pragma once

#include "GraphicManager/TextureManager/Texture.h"
#include "PostEffectPipeline/BasePostEffectPipeline.h"


/// ===================================================
/// layer毎に持たせるpost effectのクラス
/// ===================================================
class PostEffect {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PostEffect();
	~PostEffect();

	void Initialize();

	void Execution();

	void AddPipeline(BasePostEffectPipeline* _postEffectPipeline);

	void SetLayerRenderTexture(class RenderTexture* _renderTexture);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::array<Texture*, 2>              intermediateTextures_;
	std::vector<BasePostEffectPipeline*> postEffectPipelines_;

	class RenderTexture*                 pLayerRenderTexture_ = nullptr;

};