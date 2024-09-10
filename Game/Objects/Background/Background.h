#pragma once

#include <memory>
#include <GameObjectManager.h>
#include <Sprite.h>

class Background : public BaseGameObject {
public:
	Background() { CreateTag(this); }
	~Background() {}

	void Initialize() override;
	void Update() override;
	void BackSpriteDraw() override;

	void Debug() override;

	void SetColor(const Vector4& _color) { color = _color; }
private:
	std::unique_ptr<Sprite> sprite_;
	float angle_ = 0.0f;
	Vec4 color = { 0,0,0,1 };
};

