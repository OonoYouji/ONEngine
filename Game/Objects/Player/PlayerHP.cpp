#include "PlayerHP.h"

#include <SceneManager.h>
#include <ImGuiManager.h>
#include <Input.h>

#include "Enemy/Enemy.h"


void PlayerHP::Initialize() {
	maxHP_ = 3;
	currentHP_ = maxHP_;

	offset_ = Vec3(150.0f, 100.0f, 0.0f);
	distance_ = 140.0f;

	hpSprites_.resize(maxHP_);
	for(auto& sprite : hpSprites_) {
		sprite.reset(new Sprite);
		sprite->Initialize("Heart1", "Heart1.png");
		sprite->SetSize({ 64,64 });
	}
}

void PlayerHP::Update() {
	std::list<BaseGameObject*> objects = (GameObjectManager::GetGameObjectList("Enemy"));
	std::list<Enemy*> enemies;
	fluctuationHP_ = false;
	for(auto& gameObject : objects) {
		if(Enemy* enemy = dynamic_cast<Enemy*>(gameObject)) {
			enemies.push_back(enemy);
		}
	}

	
	for(auto& enemy : enemies) {
		if(enemy->IsHeartBreak()) {
			if(static_cast<uint32_t>(hpSprites_.size()) > 0) {
				hpSprites_.pop_back();
				enemy->SetHeartBreak(false);
				fluctuationHP_ = true;
			}
		}
	}

	objects.clear();
	enemies.clear();

	/// 座標初期化
	for(uint32_t i = 0U; i < static_cast<uint32_t>(hpSprites_.size()); ++i) {
		Vec3 position = {
			distance_ * i,
			0.0f,
			0.0f
		};
		position += offset_;
		hpSprites_[i]->SetPos(position);
	}

	if(hpSprites_.size() == 0) {
		///
		/// ここでresultに遷移
		///
		SceneManager::GetInstance()->SetNextScene(SCENE_ID::RESULT);
	}
}

void PlayerHP::FrontSpriteDraw() {
	for(auto& sprite : hpSprites_) {
		sprite->Draw(2);
	}
}

void PlayerHP::Debug() {


	/// デバッグ用 : ダメージを食らう
	if(Input::TriggerKey(KeyCode::F12)) {
		if(static_cast<uint32_t>(hpSprites_.size()) > 0) {
			hpSprites_.pop_back();
			//enemy->SetHeartBreak(false);
			fluctuationHP_ = true;
		}
	}


	if(ImGui::TreeNodeEx("status", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat2("offset", &offset_.x);
		ImGui::DragFloat("distance", &distance_);
		ImGui::DragInt("currentHP", reinterpret_cast<int*>(&currentHP_), 0);
		ImGui::DragInt("maxHP", reinterpret_cast<int*>(&maxHP_), 0);

		ImGui::TreePop();
	}
}

bool PlayerHP::GetHPFluctuation() {
	return fluctuationHP_;
}
