#pragma once

#include <string>
#include <RenderTexture.h>


enum LAYER_NUMBER {
	BACK_SPRITE,
	OBJECT3D,
	FRONT_SPRITE,
	LAYERNUM_COUNTER,
};


class BaseLayer {
public:

	BaseLayer();
	virtual ~BaseLayer() = default;

	void BaseInitialize();

	/*virtual void BackSpriteDraw() = 0;
	virtual void Object3dDraw() = 0;
	virtual void FrontSpriteDraw() = 0;*/

	void Draw();

	RenderTexture* GetRenderTexture(LAYER_NUMBER layerNumber) {
		return renderTextures_[layerNumber].get();
	}

	RenderTexture* GetFinalRenderTexture() {
		return finalRenderTex_.get();
	}

protected:
	static int sInstanceCount_;

	//class BaseCamera* camera_ = nullptr;
	class BaseScene* currentScene_ = nullptr;
	std::unique_ptr<RenderTexture> renderTextures_[LAYERNUM_COUNTER];
	std::unique_ptr<RenderTexture> finalRenderTex_;
	std::string className_;
	int id_;
};