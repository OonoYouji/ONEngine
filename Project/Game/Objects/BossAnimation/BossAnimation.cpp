#include "BossAnimation.h"

/// engine
#include <imgui.h>
#include "FrameManager/Time.h"


BossAnimation::BossAnimation() {
	CreateTag(this);
}

BossAnimation::~BossAnimation() {}

void BossAnimation::Initialize() {

	/// 最初のアニメーションを設定
	currentAnimationIndex_ = BOSS_ANIMATION_ENTRY;


	modelNameArray_ = {
		"bossMainBody",
		"bossTubu",
		"bossHead",
	};


	bossPartsArray_.resize(BOSS_PARTS_COUNT);
	for(int i = 0; i < BOSS_PARTS_COUNT; ++i) {
		bossPartsArray_[i] = new BossParts;
		bossPartsArray_[i]->SetModelName(modelNameArray_[i]);
		bossPartsArray_[i]->Initialize();

		if(i == 0) {
			bossPartsArray_[i]->SetParent(pTransform_);
		} else {
			bossPartsArray_[i]->SetParent(bossPartsArray_[i - 1]->GetTransform());
		}
		bossPartsArray_[i]->SetName(modelNameArray_[i]);
	}

	/// アニメーションのデータを初期化
	animationDataArray_.resize(BOSS_ANIMATION_COUNT);
	for(int i = 0; i < BOSS_ANIMATION_COUNT; ++i) {
		animationDataArray_[i] = { 0.0f, 1.0f, 1.0f }; /// 固定値で初期化
	}

	/// 動きの定義を作成
	animationUpdateFunction_.resize(BOSS_ANIMATION_COUNT);

	/// ---------------------------------------------------
	/// 何もしないうごき
	/// ---------------------------------------------------
	animationUpdateFunction_[BOSS_ANIMATION_NONE] = [&](AnimationData& data) {
		BossParts* head = bossPartsArray_[BOSS_PARTS_HEAD];
		BossParts* tubu = bossPartsArray_[BOSS_PARTS_TUBU];

		tubu->SetPosition({ 0, 0, -2.5f });
		head->SetPosition({ 0, 0, -1.5f });
	};


	/// ---------------------------------------------------
	/// 出現時の動き
	/// ---------------------------------------------------
	animationUpdateFunction_[BOSS_ANIMATION_ENTRY] = [&](AnimationData& data) {
		BossParts* head = bossPartsArray_[BOSS_PARTS_HEAD];
		BossParts* tubu = bossPartsArray_[BOSS_PARTS_TUBU];

		float value      = std::sin(data.time * data.speed) * 0.5f + 0.5f;
		float posistionY = 0.8f * value;
		float rotateX    = 0.35f * value;

		tubu->SetPosition({ 0, posistionY, -2.5f });
		tubu->SetRotateX(rotateX);

		head->SetPosition({ 0, 0, -1.5f }); /// ここでは固定
	};


}

void BossAnimation::Update() {

	AnimationData& data = animationDataArray_[currentAnimationIndex_];
	data.time += Time::DeltaTime();

	animationUpdateFunction_[currentAnimationIndex_](data);
}

void BossAnimation::Debug() {
	if(ImGui::TreeNodeEx("animation")) {

		ImGui::SliderInt("current animation index", &currentAnimationIndex_, 0, BOSS_ANIMATION_COUNT - 1);

		ImGui::SeparatorText("animation data");
		AnimationData& data = animationDataArray_[currentAnimationIndex_];
		
		ImGui::DragFloat("time",      &data.time,      0.01f);
		ImGui::DragFloat("speed",     &data.speed,     0.01f);
		ImGui::DragFloat("amplitude", &data.apmlitude, 0.01f);


		ImGui::TreePop();
	}
}
