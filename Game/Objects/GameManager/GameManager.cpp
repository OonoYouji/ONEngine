#include "GameManager.h"

#include <ImGuiManager.h>
#include <Input.h>


void GameManager::Initialize() {

	std::vector<std::string> tags{
		"Heart", "Hand", "GameMonitor",
		"GameTimer", "SinWaveDrawer",
		"Enemy", "EnemyManager",
	};

	/// tag からオブジェクトを検索してpausedObjectsに追加
	for(auto& tag : tags) {
		std::list<BaseGameObject*> add = GameObjectManager::GetGameObjectList(tag);
		pausedObjects_.splice(pausedObjects_.end(), add);
	}


	isPouse_ = false;

	sprite_.reset(new Sprite);
	sprite_->Initialize("pause", "pause.png");
	sprite_->SetPos({ 640, 360, 0 });
	spriteSize_ = { 400.0f, 64.0f };
	sprite_->SetSize(spriteSize_);

	white2x2_.reset(new Sprite);
	white2x2_->Initialize("white2x2", "white2x2.png");
	white2x2_->SetPos({ 640, 360, 0 });
	white2x2_->SetSize({ 640.0f, 360.0f });

	white2x2Color_ = Vec4(120, 120, 120, 120) / 255.0f;
	white2x2Color_.w = 70.0f / 255.0f;
	white2x2_->SetColor(white2x2Color_);


}

void GameManager::Update() {

	prevIsPause_ = isPouse_;
	isPouse_ = false;
	isPouse_ |= Input::PressKey(KeyCode::Enter);
	isPouse_ |= Input::PressPadButton(PadCode::RightShoulder);

	/// 無効なポインタの削除
	for(auto itr = pausedObjects_.begin(); itr != pausedObjects_.end();) {
		if(GameObjectManager::IsAliveObject(*itr)) {
			itr++;
		} else {
			itr = pausedObjects_.erase(itr);
		}
	}


	/// ポーズ中は他のオブジェクトを止める
	if(isPouse_) {
		for(auto& object : pausedObjects_) {
			object->isActive = false;
			for(auto& child : object->GetChilds()) {
				child->isActive = false;
			}
		}
	} else {

		/// ポーズが解かれたら元に戻す
		if(prevIsPause_) {
			for(auto& object : pausedObjects_) {
				object->isActive = true;
				for(auto& child : object->GetChilds()) {
					child->isActive = true;
				}
			}
		}

	}

}

void GameManager::FrontSpriteDraw() {
	if(isPouse_) {
		sprite_->Draw(10);
		white2x2_->Draw(9);
	}
}

void GameManager::Debug() {
	if(ImGui::TreeNodeEx("pauseSprite", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat2("size", &spriteSize_.x);
		ImGui::ColorEdit4("color", &spriteColor_.x);

		ImGui::TreePop();
	}

	if(ImGui::TreeNodeEx("white2x2Sprite", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::ColorEdit4("color", &white2x2Color_.x);

		ImGui::TreePop();
	}

	sprite_->SetSize(spriteSize_);
	sprite_->SetColor(spriteColor_);

	white2x2_->SetColor(white2x2Color_);

}

void GameManager::AddPausedObject(BaseGameObject* object) {
	pausedObjects_.push_back(object);
}
