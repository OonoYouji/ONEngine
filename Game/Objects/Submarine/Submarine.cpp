#include "Submarine.h"

#include <ImGuiManager.h>
#include <ModelManager.h>
#include <Input.h>

#include "Collision/BaseCollider.h"
#include "Collision/BoxCollider.h"

#include "Wire/Wire.h"


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
				startPos_ = Input::MouseRay(0.0f);
				startPos_.z = GetPosition().z;

				isEnter_ = true;
			}

		}
	}


	/// ---------------------------------------- ///
	///		左クリックをした後の処理
	/// ---------------------------------------- ///
	if(isEnter_) {

		/// ---------------------------------------- ///
		///		マウスの左ボタンを離した後の処理
		/// ---------------------------------------- ///
		if(Input::ReleaseMouse(MouseCode::Left)) {

			/// 終点の決定
			endPos_ = Input::MouseRay(0.0f);

			/// 垂直に伸ばすのでy,zはstartと同じ
			endPos_.y = startPos_.y; 
			endPos_.z = startPos_.z;

			(new Wire)->Initialize();

			isExit_ = true;
			isEnter_ = false;
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
	if(ImGui::TreeNodeEx("mouse ray", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat3("start position", &startPos_.x);
		ImGui::DragFloat3("end   position", &endPos_.x);

		ImGui::Spacing();

		ImGui::Checkbox("isEnter", &isEnter_);
		ImGui::Checkbox("isExit", &isExit_);


		ImGui::TreePop();
	}
}
