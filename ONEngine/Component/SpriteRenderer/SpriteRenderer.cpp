#include "SpriteRenderer.h"

#include <GameObjectManager.h>

void SpriteRenderer::Initialize() {

	pTransform_ = GetParent()->GetComponent<Transform>();

	sprite_.reset(new Sprite);
	sprite_->Initialize("white2x2", "white2x2.png");
}

void SpriteRenderer::Update() {
	sprite_->SetPos(pTransform_->position);
	sprite_->SetSize({ pTransform_->scale.x, pTransform_->scale.y });
	sprite_->SetAngle(pTransform_->rotate.z);
}

void SpriteRenderer::BackSpriteDraw() {
	if(type_ == SpriteRenderType::Back) {
		sprite_->Draw();
	}
}

void SpriteRenderer::FrontSpriteDraw() {
	if(type_ == SpriteRenderType::Front) {
		sprite_->Draw();
	}
}




void SpriteRenderer::SetTexture(const std::string& filePath) {
	size_t dotPosition = filePath.find('.');
	std::string name = filePath;

	if(dotPosition != std::string::npos) {
		name.erase(dotPosition);
	}

	sprite_->SetTexture(name, filePath);
}
