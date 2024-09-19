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


	maxGauge_ = 120.0f;
	currentGauge_ = 0.0f;
	baseGauge_ = 1.0f;
	decreaseTime_ = 3.0f;

	deathSE_ = new AudioSource;
	deathSE_->SetAudioClip("glass_parin.wav");


	texNames_ = { "dekai", "chodekai" };
	filePaths_ = { "dekai.png", "chodekai.png" };
	texSizes_ = {
		Vec2(400, 120) / 3.0f,
		Vec2(500, 120) / 3.0f
	};

	evalLifeTime_ = 1.0f;

	evals_.resize(6);
	for(auto& eval : evals_) {
		eval.sprite.reset(new Sprite);
		eval.sprite->Initialize(texNames_[0], filePaths_[0]);
		eval.lifeTime = 0.0f;
	}

	posYs_.resize(6);
	for(uint32_t i = 0u; i < 6u; ++i) {
		posYs_[i] = 400.0f - i * 60.0f;
	}

	evalSEs_[BIG] = new AudioSource;
	evalSEs_[SUPER_BIG] = new AudioSource;

	evalSEs_[BIG]->SetAudioClip("dekai.wav");
	evalSEs_[SUPER_BIG]->SetAudioClip("chodekai.wav");

}

void PlayerHP::Update() {
	std::list<BaseGameObject*> objects = (GameObjectManager::GetGameObjectList("Enemy"));
	std::list<Enemy*> enemies;
	fluctuationHP_ = false;
	healHP_ = false;
	for(auto& gameObject : objects) {
		if(Enemy* enemy = dynamic_cast<Enemy*>(gameObject)) {
			enemies.push_back(enemy);
		}
	}

	if (decreaseTime_ > 0) {
		decreaseTime_ -= WorldTime::DeltaTime();
	}
	else if (decreaseTime_ <= 0) {
		decreaseTime_ = 3.0f;
		currentGauge_--;
		if (currentGauge_ <= 0) {
			currentGauge_ = 0;
		}
	}

	for(auto& enemy : enemies) {
		if(enemy->IsHeartBreak()) {

			/// TODO: 同じ敵で何回かダメージを食らうことがある
			currentDamegeIndex_++;
			fluctuationHP_ = true;
			currentDamegeIndex_ = std::min(currentDamegeIndex_, 4);
			enemy->SetHeartBreak(false);

		}

		if(enemy->IsScore()) {
			enemy->SetIsScore(false);
			int medicSize = enemy->GetMedicSize();
			int magnification = enemy->GetMagnification();
			float addScoreGauge = baseGauge_ * (float)magnification;
			currentGauge_ += addScoreGauge;
			healHP_ = true;
			if(currentGauge_ >= maxGauge_) {
				currentGauge_ = maxGauge_;
			}

			/// デカい、ちょーでかいの評価を出す
			if(medicSize >= 3) {

				for(auto& eval : evals_) {
					if(eval.lifeTime <= 0.0f) {
						eval.lifeTime = evalLifeTime_;
						eval.posY = enemy->GetPos().y;
						eval.posY = posYs_[Random::Int(0, 5)];;
						eval.sprite->SetTexture(
							texNames_[medicSize - 3],
							filePaths_[medicSize - 3]
						);
						eval.sprite->SetSize(texSizes_[medicSize - 3]);

						evalSEs_[medicSize - 3]->volume = 5.0f;
						evalSEs_[medicSize - 3]->PlayAudio();
						break;
					}
				}

			}
		}

	}

	objects.clear();
	enemies.clear();

	if(currentDamegeIndex_ == 4) {
		/// ここでResult (Game Over)に行く
		deathSE_->PlayAudio();
		SceneManager::GetInstance()->SetNextScene(SCENE_ID::RESULT);
	}


	CalculationGage();
	EvalsUpdate();



	if(maxGauge_ == currentGauge_) {

		///
		/// この部分でclearへ
		///
		SceneManager::GetInstance()->SetNextScene(SCENE_ID::CLEAR);

	}

}

void PlayerHP::FrontSpriteDraw() {

	gaugeSprite_->Draw();

	frameSprite_[currentDamegeIndex_]->Draw();

	for(auto& eval : evals_) {
		if(eval.lifeTime > 0.0f) {
			eval.sprite->Draw();
		}
	}

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

bool PlayerHP::GetHPHeal() {
	return healHP_;
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

void PlayerHP::EvalsUpdate() {
	for(auto& eval : evals_) {

		if(eval.lifeTime == 0.0f) {
			continue;
		}

		eval.lifeTime -= WorldTime::DeltaTime();
		float lerpT = 2.0f - std::max(eval.lifeTime / (evalLifeTime_ / 2.0f), 0.0f);

		if(lerpT <= 1.0f) {
			float posX = std::lerp(-200.0f, 200.0f, Ease::Out::Back(lerpT));
			eval.sprite->SetPos({ posX, eval.posY, 0.0f });
		} else {
			float posX = std::lerp(200.0f, -200.0f, Ease::In::Back(lerpT - 1.0f));
			eval.sprite->SetPos({ posX, eval.posY, 0.0f });
		}

		if(lerpT == 2.0f) {
			eval.lifeTime = 0.0f;
		}

	}
}
