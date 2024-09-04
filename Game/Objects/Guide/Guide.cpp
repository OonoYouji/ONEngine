#define NOMINMAX
#include "Guide.h"

#include <algorithm>

#include <CameraManager.h>
#include <Input.h>

#include "Submarine/Submarine.h"



void Guide::Initialize() {
	sprite_.reset(new Sprite);
	sprite_->Initialize("uvChecker", "uvChecker.png");
}

void Guide::Update() {
	if(submarine_) {

		/// ---------------------------------------- ///
		///		スプライトのスクリーン座標を計算
		/// ---------------------------------------- ///

		submarine_->GetCollider();

		/// スクリーン座標
		Vec2 screenPos = Input::MousePosition();
		SetPosition({ screenPos.x, screenPos.y, 1.0f });

	}

	sprite_->SetPos(GetPosition());
}

void Guide::Draw() {
	sprite_->Draw();
}



void Guide::SetSubmarine(Submarine* submarine) {
	submarine_ = submarine;
}
