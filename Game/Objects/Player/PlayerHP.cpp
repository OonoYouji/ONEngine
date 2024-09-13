#define	NOMINMAX
#include "PlayerHP.h"

#include <SceneManager.h>
#include <ImGuiManager.h>
#include <Input.h>
#include <WorldTime.h>
#include <Easing.h>

#include "Enemy/Enemy.h"
#include "LineDrawer2D/SinWaveDrawer.h"


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

	pWave_ = dynamic_cast<SinWaveDrawer*>(GameObjectManager::GetGameObject("SinWaveDrawer"));

	frameColor_ = Vec4(100, 100, 100, 255) / 255.0f;
	gaugeColor_ = Vec4(0.184f, 0.851f, 0.137f, 1.0f);

	frameSprite_.resize(4);
	for(auto& sprite : frameSprite_) {
		sprite.reset(new Sprite);
		sprite->Initialize("gauge", "gauge.png");
		sprite->SetPos({ 640.0f, 100.0f, 0.0f });
		sprite->SetSize(Vec2(1280.0f, 200.0f) / 2.5f);
		sprite->SetColor(frameColor_);
	}

	gaugeSprite_.reset(new Sprite);
	gaugeSprite_->Initialize("white2x2", "white2x2.png");



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


	//if(hpSprites_.size() == 0) {
	//	SceneManager::GetInstance()->SetNextScene(SCENE_ID::RESULT);
	//}


	CalculationGage();

}

void PlayerHP::FrontSpriteDraw() {
	/*for(auto& sprite : hpSprites_) {
		sprite->Draw(2);
	}*/


	gaugeSprite_->Draw();
	for(auto& sprite : frameSprite_) {
		sprite->Draw();
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

	if(ImGui::TreeNodeEx("frame", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::ColorEdit4("color", &frameColor_.x);

		ImGui::TreePop();
	}


	if(ImGui::TreeNodeEx("gauge", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat("currentGauge", &currentGauge_, 0.1f);
		ImGui::DragFloat("maxGauge", &maxGauge_, 0.1f);

		ImGui::ColorEdit4("color", &gaugeColor_.x);

		ImGui::TreePop();
	}

}

bool PlayerHP::GetHPFluctuation() {
	return fluctuationHP_;
}

void PlayerHP::CalculationGage() {

	/// フレームの色をセット
	for(auto& sprite : frameSprite_) {
		sprite->SetColor(frameColor_);
	}

	float lerpT = std::min(currentGauge_ / maxGauge_, 1.0f);

	gaugeSprite_->SetPos(Vec3(
		std::lerp(640.0f - (952.0f / 2.0f), 640.0f, lerpT),
		100.0f, 0.0f
	));

	gaugeSprite_->SetSize(Vec2(
		std::lerp(0.0f, 952.0f / 2.0f, lerpT),
		60.0f / 2.0f
	));

	gaugeSprite_->SetColor(gaugeColor_);


}
