#pragma once

#include <string>

#include <RenderTexture.h>
#include <Bloom/Bloom.h>
#include <Vector2.h>


enum LAYER_NUMBER {
	BACK_SPRITE,
	OBJECT3D,
	FRONT_SPRITE,
	LAYERNUM_COUNTER,
};


class SceneLayer final {
public:

	SceneLayer();
	~SceneLayer() {}

	void Initialize(const std::string& className, class BaseCamera* camera);

	void Draw();

	void ImGuiDebug();


	void SetIsApplyBloom(bool isApplyBloom, LAYER_NUMBER layerNumber);

	void SetTexSize(const Vec2& texSize, LAYER_NUMBER layerNumber);
	void SetIntensity(float intensity, LAYER_NUMBER layerNumber);
	void SetRadius(int radius, LAYER_NUMBER layerNumber);


	RenderTexture* GetRenderTexture(LAYER_NUMBER layerNumber) {
		return renderTextures_[layerNumber].get();
	}

	RenderTexture* GetFinalRenderTexture() {
		return finalRenderTex_.get();
	}

protected:
	static int sInstanceCount_;

	class BaseCamera* camera_ = nullptr;
	class BaseScene* currentScene_ = nullptr;
	std::unique_ptr<RenderTexture> renderTextures_[LAYERNUM_COUNTER];
	std::unique_ptr<RenderTexture> finalRenderTex_;
	std::string className_;
	int id_;

	bool isApplyBlooms_[LAYERNUM_COUNTER]{};
	std::unique_ptr<Bloom> blooms_[LAYERNUM_COUNTER];
};