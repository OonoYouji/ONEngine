#pragma once

#include <string>

#include "GraphicManager/RenderTextureManager/RenderTexture.h"
#include "GraphicManager/PostEffect/Bloom/Bloom.h"
#include "Math/Vector2.h"

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


	RenderTexture* GetRenderTexture() {
		return renderTexture_.get();
	}

protected:
	static int sInstanceCount_;

	class BaseCamera* camera_ = nullptr;
	class BaseScene*  currentScene_ = nullptr;
	std::unique_ptr<RenderTexture> renderTexture_;
	std::string className_;
	int id_;

	bool isApplyBlooms_[LAYERNUM_COUNTER]{};
	std::unique_ptr<Bloom> blooms_[LAYERNUM_COUNTER];
};