#include "GameManager.h"

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

void GameManager::AddPausedObject(BaseGameObject* object) {
	pausedObjects_.push_back(object);
}
