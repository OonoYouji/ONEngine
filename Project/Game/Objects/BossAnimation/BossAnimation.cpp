#define NOMINMAX
#include "BossAnimation.h"

/// engine
#include <imgui.h>
#include "FrameManager/Time.h"

/// components
#include "ComponentManager/AudioSource/AudioSource.h"

/// objects
#include "Objects/Camera/GameCamera.h"

/// math
#include "Math/Easing.h"


BossAnimation::BossAnimation() {
	CreateTag(this);
}

BossAnimation::~BossAnimation() {}

void BossAnimation::Initialize() {

	audioSource_ = AddComponent<AudioSource>();

	/// 最初のアニメーションを設定
	currentAnimationIndex_ = BOSS_ANIMATION_STANDBY;


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

	/// ===================================================
	/// アニメーションの時間の設定
	/// ===================================================

	animationDataArray_[BOSS_ANIMATION_ENTRY_CAMERA_MOVE] = { 0.0f, 5.0f, 1.0f };
	animationDataArray_[BOSS_ANIMATION_ENTRY_RAISE_TUBE] = { 0.0f, 1.5f, 1.0f };
	animationDataArray_[BOSS_ANIMATION_ENTRY_TUBE_DOWN]  = { 0.0f, 0.75f, 1.0f };




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

		tubu->SetPosition({ 0, 0, -2.7f });
		head->SetPosition({ 0, 0, -2.0f });
	};
	
	/// ---------------------------------------------------
	/// 待機モーション
	/// ---------------------------------------------------
	animationUpdateFunction_[BOSS_ANIMATION_STANDBY] = [&](AnimationData& data) {
		BossParts* head = bossPartsArray_[BOSS_PARTS_HEAD];
		BossParts* tubu = bossPartsArray_[BOSS_PARTS_TUBU];

		Vec3 tubePosition = {
			0.0f,
			std::sin(data.time) * 0.1f,
			-2.7f
		};

		tubu->SetPosition(tubePosition);
		head->SetPosition({ 0, 0, -2.0f });
	};


	/// ---------------------------------------------------
	/// 出現時の動き no1 カメラ移動
	/// ---------------------------------------------------
	animationUpdateFunction_[BOSS_ANIMATION_ENTRY_CAMERA_MOVE] = [&](AnimationData& data) {
		BossParts* head = bossPartsArray_[BOSS_PARTS_HEAD];
		BossParts* tubu = bossPartsArray_[BOSS_PARTS_TUBU];
		tubu->SetPosition({ 0, 0, -2.7f });
		head->SetPosition({ 0, 0, -2.0f });


		/// time / seconds
		float lerpT = std::min(data.time / data.maxTime, 1.0f);

		Vec3 cameraPosition = Vec3::Lerp(
			{ -4.2f, 4.5f + 11.0f, -9.15f },       /// スタート位置 
			{ -4.2f, 3.0f + 11.0f, -9.15f },       /// 終了位置
			/*Ease::In::Circ*/(lerpT)
		);

		Vec3 cameraRotate = Vec3::Lerp(
			{ 0.0f, 0.566f, 0.0f },       /// スタート位置 
			{ 0.3f, 0.566f, 0.0f },       /// 終了位置
			/*Ease::In::Circ*/(lerpT)
		);


		Vec3 shake = {
			std::sin(data.time * 50.0f) * 0.1f * (1.0f - lerpT),
			0.0f, 0.0f
		};

		shake = Mat4::Transform(shake, Mat4::MakeRotate(cameraRotate));

		pGameCamera_->SetPosition(cameraPosition + shake);
		pGameCamera_->SetRotate(cameraRotate);

		
		if(lerpT == 1.0f) {
			currentAnimationIndex_ = BOSS_ANIMATION_ENTRY_RAISE_TUBE;
		}

		animationUpdateFunction_[BOSS_ANIMATION_STANDBY](data);
	};
	

	/// ---------------------------------------------------
	/// 出現時の動き no2 チューブの振り上げ
	/// ---------------------------------------------------
	animationUpdateFunction_[BOSS_ANIMATION_ENTRY_RAISE_TUBE] = [&](AnimationData& data) {
		BossParts* head = bossPartsArray_[BOSS_PARTS_HEAD];
		BossParts* tubu = bossPartsArray_[BOSS_PARTS_TUBU];


		float lerpT = std::min(data.time / data.maxTime, 1.0f);
		
		float posistionY = 0.8f * lerpT;
		float rotateX = 0.35f * lerpT;

		Vec3 tubuPosition = Vec3::Lerp(
			{ 0.0f, 0.0f, -2.7f }, /// スタート位置
			{ 0.0f, 0.8f, -2.7f }, /// 終了位置
			Ease::InOut::Elastic(lerpT)
		);
		
		Vec3 tubuRotate = Vec3::Lerp(
			{ 0.0f, 0.0f, 0.0f },  /// スタート時の回転各
			{ 0.35f, 0.0f, 0.0f }, /// 終了時の回転角
			Ease::InOut::Elastic(lerpT)
		);

		tubu->SetPosition(tubuPosition);
		tubu->SetRotate(tubuRotate);

		head->SetPosition({ 0, 0, -2.0f }); /// ここでは固定


		if(!data.isPlayAudio) {
			audioSource_->PlayOneShot("bossRaiseTube.wav", 0.5f);
			data.isPlayAudio = true;
		}

		/// 次の動き
		if(lerpT == 1.0f) {
			currentAnimationIndex_ = BOSS_ANIMATION_ENTRY_TUBE_DOWN;
		}

	};
	

	/// ---------------------------------------------------
	/// 出現時の動き no3 チューブの振り下ろし
	/// ---------------------------------------------------
	animationUpdateFunction_[BOSS_ANIMATION_ENTRY_TUBE_DOWN] = [&](AnimationData& data) {
		BossParts* head = bossPartsArray_[BOSS_PARTS_HEAD];
		BossParts* tubu = bossPartsArray_[BOSS_PARTS_TUBU];


		float lerpT = std::min(data.time / data.maxTime, 1.0f);
		
		float posistionY = 0.8f * lerpT;
		float rotateX = 0.35f * lerpT;

		Vec3 tubuPosition = Vec3::Lerp(
			{ 0.0f, 0.8f, -2.7f }, /// スタート位置
			{ 0.0f, 0.0f, -2.7f }, /// 終了位置
			Ease::InOut::Elastic(lerpT)
		);
		
		Vec3 tubuRotate = Vec3::Lerp(
			{ 0.35f, 0.0f, 0.0f },  /// スタート時の回転各
			{ 0.0f, 0.0f, 0.0f }, /// 終了時の回転角
			Ease::InOut::Elastic(lerpT)
		);

		tubu->SetPosition(tubuPosition);
		tubu->SetRotate(tubuRotate);

		head->SetPosition({ 0, 0, -2.0f }); /// ここでは固定

		/// 次の動き
		if(lerpT >= 0.6f) {
			
			if(!data.isPlayAudio) {
				/// 一回だけ鳴らす
				audioSource_->PlayOneShot("bossRaiseTube.wav", 0.5f);
				data.isPlayAudio = true;
			}

			/// カメラをシェイクさせる
			float cameraLerpT = std::min((data.time - data.maxTime) / 0.5f, 1.0f);

			Vec3 shake = {
				0.0f,
				std::sin(data.time * 50.0f) * 0.1f * (1.0f - cameraLerpT),
				0.0f
			};

			Vec3 cameraRotate   = { 0.3f, 0.566f, 0.0f };
			Vec3 cameraPosition = { -4.2f, 3.0f + 11.0f, -9.15f };

			shake = Mat4::Transform(shake, Mat4::MakeRotate(cameraRotate));

			pGameCamera_->SetPosition(cameraPosition + shake);

			if(cameraLerpT == 1.0f) {
				currentAnimationIndex_ = BOSS_ANIMATION_STANDBY;
			}
		}

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
		ImGui::DragFloat("max time",  &data.maxTime,   0.01f);
		ImGui::DragFloat("amplitude", &data.apmlitude, 0.01f);

		if(ImGui::Button("reset")) {
			data.time = 0.0f;
		}

		if(ImGui::Button("reset all")) {
			for(auto& animationData : animationDataArray_) {
				animationData.time = 0.0f;
			}
		}

		ImGui::TreePop();
	}
}

void BossAnimation::SetGameCamera(GameCamera* _gameCamera) {
	pGameCamera_ = _gameCamera;
}

void BossAnimation::SetAnimationIndex(BOSS_ANIMATION_ORDER _bossAnimationOrder) {
	currentAnimationIndex_ = _bossAnimationOrder;
}

const AnimationData& BossAnimation::GetAnimatinData(BOSS_ANIMATION_ORDER bossAnimatinOrder) {
	return animationDataArray_[bossAnimatinOrder];
}
