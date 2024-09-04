#include "Submarine.h"

#include <ImGuiManager.h>
#include <ModelManager.h>
#include <Input.h>


/// <summary>
/// 初期化
/// </summary>
void Submarine::Initialize() {
	model_ = ModelManager::CreateCube();



}


/// <summary>
/// 更新処理
/// </summary>
void Submarine::Update() {

	/// オブジェクト上でマウスをクリックし始めたか
	if(Input::TriggerMouse(MouseCode::Left)) {

	}

	mouseRayPosition_ = Input::MouseRay(distance_);

	

}


/// <summary>
/// 描画
/// </summary>
void Submarine::Draw() {
	model_->Draw(&transform_);
}

void Submarine::Debug() {
	if(ImGui::TreeNodeEx("mouse ray")) {

		ImGui::DragFloat("distance", &distance_, 0.1f);
		ImGui::DragFloat3("ray position", &mouseRayPosition_.x, 0.1f);

		Vec3 nearPos = Input::MouseNearPosition();
		ImGui::DragFloat3("ray position", &nearPos.x, 0.1f);

		Vec3 farPos = Input::MouseFarPosition();
		ImGui::DragFloat3("ray position", &farPos.x, 0.1f);

		ImGui::TreePop();
	}
}
