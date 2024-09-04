#include "Submarine.h"

#include <ImGuiManager.h>
#include <ModelManager.h>
#include <CameraManager.h>
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

				/// カメラとの距離をMouseRayのdistanceに使用する
				float distance =
					Vec3::Length(CameraManager::GetInstance()->GetMainCamera()->GetPosition() - GetPosition());

				/// 始点の決定
				startPos_ = Input::MouseRay(distance);
				startPos_.z = GetPosition().z;

				newWire_ = new Wire();
				newWire_->Initialize();

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

			/// カメラとの距離をMouseRayのdistanceに使用する
			float distance =
				Vec3::Length(CameraManager::GetInstance()->GetMainCamera()->GetPosition() - GetPosition());

			/// 終点の決定
			endPos_ = Input::MouseRay(distance);

			/// 垂直に伸ばすのでx,zはstartと同じ
			endPos_.x = startPos_.x;
			endPos_.z = startPos_.z;

			Vec3 startPos = {
				startPos_.x,
				GetPosition().y,
				startPos_.z
			};

			newWire_->SetPosition(Vec3::Lerp(startPos, endPos_, 0.5f));
			newWire_->UpdateMatrix();
			newWire_->SetScaleY(std::abs(GetPosition().y - newWire_->GetPosition().y));

			newWire_ = nullptr;

			isExit_ = true;
			isEnter_ = false;
		}


		/// ---------------------------------------- ///
		///		ワイヤーの位置とサイズの調整
		/// ---------------------------------------- ///

		if(newWire_) {

			Vec3 startPos = {
				startPos_.x,
				GetPosition().y,
				startPos_.z
			};

			/// カメラとの距離をMouseRayのdistanceに使用する
			float distance =
				Vec3::Length(CameraManager::GetInstance()->GetMainCamera()->GetPosition() - GetPosition());

			Vec3 endPos = Input::MouseRay(distance);

			/// 垂直に伸ばすのでx,zはstartと同じ
			endPos.x = startPos_.x;
			endPos.z = startPos_.z;

			/// position scaleの計算
			newWire_->SetPosition(Vec3::Lerp(startPos, endPos, 0.5f));
			newWire_->UpdateMatrix();
			newWire_->SetScaleY(std::abs(GetPosition().y - newWire_->GetPosition().y));
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

		Vec3 mouseRay = Input::MouseRay(0.f);
		ImGui::DragFloat3("mouse ray position", &mouseRay.x);

		ImGui::DragFloat3("start position", &startPos_.x);
		ImGui::DragFloat3("end   position", &endPos_.x);

		ImGui::Spacing();

		ImGui::Checkbox("isEnter", &isEnter_);
		ImGui::Checkbox("isExit", &isExit_);


		ImGui::TreePop();
	}
}
