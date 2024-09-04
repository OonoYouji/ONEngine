#include "Guide.h"

#include <CameraManager.h>
#include <Input.h>

#include "Submarine/Submarine.h"



void Guide::Initialize() {
	sprite_.reset(new Sprite);
	sprite_->Initialize("uvChecker", "uvChecker.png");
}

void Guide::Update() {
	if(submarine_) {
		float distance = Vec3::Length(submarine_->GetPosition() - CameraManager::GetInstance()->GetMainCamera()->GetPosition());
		Vec3 worldPos = Input::MouseRay(distance);
		Vec2 screenPos = Vec2::ConvertScreen(worldPos);
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
