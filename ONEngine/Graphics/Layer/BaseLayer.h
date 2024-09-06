#pragma once

#include <string>
#include <RenderTexture.h>

class BaseLayer {
public:

	virtual ~BaseLayer() = default;

	virtual void BackSpriteDraw() = 0;
	virtual void Object3dDraw() = 0;
	virtual void FrontSpriteDraw() = 0;

	void Draw();

protected:
	class BaseCamera* camera_ = nullptr;
	std::unique_ptr<RenderTexture> renderTexture_;
	std::string className_;
};