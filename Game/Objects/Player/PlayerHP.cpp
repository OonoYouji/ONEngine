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


	frameColor_ = Vec4(100, 100, 100, 255) / 255.0f;
	gaugeColor_ = Vec4(0.184f, 0.851f, 0.137f, 1.0f);

	frameSprite_.resize(5);
	for(auto& sprite : frameSprite_) {
		sprite.reset(new Sprite);
		sprite->Initialize("gauge", "gauge.png");
		sprite->SetPos({ 640.0f, 100.0f, 0.0f });
		sprite->SetSize(Vec2(1280.0f, 200.0f) / 2.5f);
		sprite->SetColor(frameColor_);
	}


	std::vector<std::string> texNames{
		"gauge", "gauge_break_low",
		"gauge_break_middle", "gauge_break_high",
		"gauge_break"
	};

	std::vector<std::string> filePaths{
		"gauge.png", "gauge_break_low.png",
		"gauge_break_middle.png", "gauge_break_high.png",
		"gauge_break.png"
	};

	for(uint32_t i = 0u; i < 5u; ++i) {
		frameSprite_[i]->SetTexture(texNames[i], filePaths[i]);
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

			/// TODO: 同じ敵で何回かダメージを食らうことがある
			currentDamegeIndex_++;
			currentDamegeIndex_ = std::min(currentDamegeIndex_, 4);
			enemy->SetHeartBreak(false);

		}
	}

	objects.clear();
	enemies.clear();

	if(currentDamegeIndex_ == 4) {
		/// ここでResult (Game Over)に行く
		SceneManager::GetInstance()->SetNextScene(SCENE_ID::RESULT);
	}


	CalculationGage();

}

void PlayerHP::FrontSpriteDraw() {

	gaugeSprite_->Draw();

	frameSprite_[currentDamegeIndex_]->Draw();

}

void PlayerHP::Debug() {


	if(ImGui::TreeNodeEx("frame", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::ColorEdit4("color", &frameColor_.x);
		ImGui::DragInt("index", &currentDamegeIndex_, 1, 0, 4);

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


	currentDamegeIndex_ = std::clamp(currentDamegeIndex_, 0, int(frameSprite_.size() - 1));



}
