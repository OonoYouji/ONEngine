#define NOMINMAX
#include "BossAnimation.h"

/// engine
#include <imgui.h>
#include "FrameManager/Time.h"

/// objects
#include "Objects/Camera/GameCamera.h"

/// math
#include "Math/Easing.h"


BossAnimation::BossAnimation() {
	CreateTag(this);
}

BossAnimation::~BossAnimation() {}

void BossAnimation::Initialize() {

	/// 最初のアニメーションを設定
	currentAnimationIndex_ = BOSS_ANIMATION_ENTRY_CAMERA_MOVE;


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
		animationDataArray_[i] = { 0.0f, 1.0f, 1.0f, false }; /// 固定値で初期化
	}

	/// 動きの定義を作成
	animationUpdateFunction_.resize(BOSS_ANIMATION_COUNT);
	for(int i = 0; i < BOSS_ANIMATION_COUNT; ++i) {
		animationUpdateFunction_[i] = [](AnimationData& data) {}; /// 固定値初期化
	}

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
	/// 出現時の動き no1 カメラ移動
	/// ---------------------------------------------------
	animationUpdateFunction_[BOSS_ANIMATION_ENTRY_CAMERA_MOVE] = [&](AnimationData& data) {
		BossParts* head = bossPartsArray_[BOSS_PARTS_HEAD];
		BossParts* tubu = bossPartsArray_[BOSS_PARTS_TUBU];
		tubu->SetPosition({ 0, 0, -2.5f });
		head->SetPosition({ 0, 0, -1.5f });


		/// time / seconds
		float lerpT = std::min(data.time / 1.0f, 1.0f); 

		Vec3 cameraPosition = Vec3::Lerp(
			{ 0.0f, 5.0f, -10.0f },       /// スタート位置 
			{ 0.0f, 0.0f, -10.0f },       /// 終了位置
			Ease::InOut::Elastic(lerpT)
		);

		pGameCamera_->SetPosition(cameraPosition);

		
		if(lerpT == 1.0f) {
			currentAnimationIndex_ = BOSS_ANIMATION_ENTRY_RAISE_TUBE;
		}
	};
	

	/// ---------------------------------------------------
	/// 出現時の動き no2 チューブの振り上げ
	/// ---------------------------------------------------
	animationUpdateFunction_[BOSS_ANIMATION_ENTRY_RAISE_TUBE] = [&](AnimationData& data) {
		BossParts* head = bossPartsArray_[BOSS_PARTS_HEAD];
		BossParts* tubu = bossPartsArray_[BOSS_PARTS_TUBU];
		tubu->SetPosition({ 0, 0, -2.5f });
		head->SetPosition({ 0, 0, -1.5f });


	};


}

void BossAnimation::Update() {

	AnimationData& data = animationDataArray_[currentAnimationIndex_];
	data.time += Time::DeltaTime();

	animationUpdateFunction_[currentAnimationIndex_](data);
}

void BossAnimation::Debug() {
	if(ImGui::TreeNodeEx("animation", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::SliderInt("current animation index", &currentAnimationIndex_, 0, BOSS_ANIMATION_COUNT - 1);

		ImGui::SeparatorText("animation data");
		AnimationData& data = animationDataArray_[currentAnimationIndex_];
		
		ImGui::DragFloat("time",      &data.time,      0.01f);
		ImGui::DragFloat("speed",     &data.speed,     0.01f);
		ImGui::DragFloat("amplitude", &data.apmlitude, 0.01f);

		if(ImGui::Button("reset")) {
			data.isReset = true;
			data.time = 0.0f;
		}

		ImGui::TreePop();
	}
}

void BossAnimation::SetGameCamera(GameCamera* _gameCamera) {
	pGameCamera_ = _gameCamera;
}
