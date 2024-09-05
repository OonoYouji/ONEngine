#include "Elevator.h"

#include <ImGuiManager.h>
#include <ModelManager.h>
#include <WorldTime.h>

#include "Wire/Wire.h"


float Elevator::sConstantSpeed_ = 0.25f;

void Elevator::Initialize() {
	model_ = ModelManager::Load("Elevator");
	speed_ = -sConstantSpeed_;
}


void Elevator::Update() {

	/// ---------------------------------------- ///
	///		wireのポインタが有効時にしか更新しない
	/// ---------------------------------------- ///
	if(!wire_) { return; }



	/// ---------------------------------------- ///
	///		座標計算
	/// ---------------------------------------- ///

	transform_.position.y += speed_ * WorldTime::DeltaTime();
	UpdateMatrix();


	
	/// ---------------------------------------- ///
	///		折り返しの計算
	/// ---------------------------------------- ///

	/// 上に着いた
	if(wire_->GetTop().y < GetPosition().y) {

		speed_ = -sConstantSpeed_;

		/// 下に着いた
	} else if(wire_->GetBottom().y > GetPosition().y) {

		speed_ = sConstantSpeed_;

	}

}


void Elevator::Draw() {
	model_->Draw(&transform_);
}

void Elevator::Debug() {
	if(ImGui::TreeNodeEx("")) {

		ImGui::DragFloat("constant speed", &sConstantSpeed_, 0.01f);

		ImGui::TreePop();
	}
}

void Elevator::SetWire(Wire* wire) {
	wire_ = wire;
}
