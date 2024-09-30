#include "SpriteRenderer.h"

#include <ImGuiManager.h>
#include <GameObjectManager.h>

void SpriteRenderer::Initialize() {

	pTransform_ = GetParent()->GetComponent<Transform>();

	sprite_.reset(new Sprite);
	sprite_->Initialize("white2x2", "white2x2.png");
	sprite_->SetTransformToPointer(pTransform_);
}

void SpriteRenderer::Update() {}

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

void SpriteRenderer::Debug() {
	if(ImGui::TreeNodeEx(GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

		Vec2 anchor = sprite_->GetAnchor();
		ImGui::DragFloat2("anchor point", &anchor.x, 0.005f);
		if(ImGui::IsItemEdited()) {
			SetAnchor(anchor);
		}


		ImGui::DragFloat2("uvSize",     &uvSize_.x,     0.01f);
		if(ImGui::IsItemEdited()) {
			SetUVSize(uvSize_);
		}

		ImGui::DragFloat2("uvPosition", &uvPosition_.x, 0.01f);
		if(ImGui::IsItemEdited()) {
			SetUVPosition(uvPosition_);
		}


		ImGui::TreePop();
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

void SpriteRenderer::SetAnchor(const Vec2& anchor) {
	sprite_->SetAnchor(anchor);
}

void SpriteRenderer::SetUVPosition(const Vec2& position) {
	sprite_->SetUVPosition(position);
}

void SpriteRenderer::SetUVSize(const Vec2& size) {
	sprite_->SetUVSize(size);
}
