#include "Submarine.h"

#include <ImGuiManager.h>
#include <ModelManager.h>
#include <Input.h>

#include "Collision/BaseCollider.h"
#include "Collision/BoxCollider.h"


/// <summary>
/// 初期化
/// </summary>
void Submarine::Initialize() {
	model_ = ModelManager::CreateCube();

	CreateBoxCollider(model_);

}


/// <summary>
/// 更新処理
/// </summary>
void Submarine::Update() {

	/// ---------------------------------------- ///
	///		Modelにマウスが被っているか
	/// ---------------------------------------- ///
	if(dynamic_cast<BoxCollider*>(GetCollider())) {
		bool isCollision = Collision::BoxToSegment(dynamic_cast<BoxCollider*>(GetCollider()), Input::MouseNearPosition(), Input::MouseFarPosition());
		if(isCollision) {

			/// ---------------------------------------- ///
			///		マウスの左ボタンをクリックしたら
			/// ---------------------------------------- ///
			if(Input::TriggerMouse(MouseCode::Left)) {

				/// 始点の決定


			}

		}
	}

}


/// <summary>
/// 描画
/// </summary>
void Submarine::Draw() {
	model_->Draw(&transform_);
}

void Submarine::Debug() {
	if(ImGui::TreeNodeEx("mouse ray")) {

		

		ImGui::TreePop();
	}
}
