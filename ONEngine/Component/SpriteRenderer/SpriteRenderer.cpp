#include "SpriteRenderer.h"

#include <ImGuiManager.h>
#include <GameObjectManager.h>

void SpriteRenderer::Initialize() {

	pTransform_ = GetParent()->GetComponent<Transform>();

	sprite_.reset(new Sprite);
	sprite_->Initialize("white2x2", "white2x2.png");
	sprite_->SetTransformToPointer(pTransform_);
}

void SpriteRenderer::Update() {
	sprite_->UpdateMatrix();
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

void SpriteRenderer::Debug() {
	if(ImGui::TreeNodeEx(GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

		Vec2 anchor = sprite_->GetAnchor();
		ImGui::DragFloat2("anchor point", &anchor.x, 0.005f);
		if(ImGui::IsItemEdited()) {
			SetAnchor(anchor);
		}

		if(ImGui::TreeNodeEx("uvTransform", ImGuiTreeNodeFlags_DefaultOpen)) {

			Vec2 uvSize = sprite_->GetUVSize();
			ImGui::DragFloat2("size", &uvSize.x, 0.01f);
			if(ImGui::IsItemEdited()) {
				SetUVSize(uvSize);
			}

			float rotate = sprite_->GetUVRotate();
			ImGui::DragFloat("rotate", &rotate, 0.01f);
			if(ImGui::IsItemEdited()) {
				SetUVRotate(rotate);
			}

			Vec2 uvPosition = sprite_->GetUVPosition();
			ImGui::DragFloat2("position", &uvPosition.x, 0.01f);
			if(ImGui::IsItemEdited()) {
				SetUVPosition(uvPosition);
			}

			ImGui::TreePop();
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

void SpriteRenderer::SetUVRotate(float rotate) {
	sprite_->SetUVRotate(rotate);
}
