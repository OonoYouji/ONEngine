#pragma once

#include <GameObjectManager.h>
#include <Sprite.h>


class GameClearEffect final : public BaseGameObject {

	enum EFFECT_PHASE {
		HAND_MOVING,			/// 手が前に出てくる
		STOPPING_WAVE,			/// 波を止める
		DRAW_THANK_YOU_LETTER,	/// 感謝状を出す
		EFFECT_END,				/// 演出の終了
		CAMERA_MOVING,			/// カメラをモニターに近づける
		WAIT,					/// 待機
		EFFECT_PHASE_COUNT
	};


public:

	GameClearEffect() { CreateTag(this); }
	~GameClearEffect() {}

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void FrontSpriteDraw() override;

	void Debug() override;


	void HandMoving();

	void CameraMoving();

	void StoppingWave();

	void DrawThankYouLetter();

	void EffectEnd();


	void SetNextPhase(EFFECT_PHASE next, float waitTime);


	void ShakeHand();


private:

	uint32_t currentPhase_ = HAND_MOVING;
	uint32_t nextPhase_ = 0u;
	float waitTime_;

	/// 手が前に出てくる
	class Hand* hand_ = nullptr;
	float handAnimationTime_ = 0.0f;
	float handMaxAnimationTime_ = 1.0f;

	float shakeHandAnimationTime_ = 0.0f;


	///  カメラをモニターに近づける

	float cameraMovingTime_ = 0.0f;
	float cameraMovingMaxTime_ = 1.0f;

	/// 波が落ち着く
	class Wave* wave_ = nullptr;
	float waveAnimationTime_ = 0.0f;
	float waveMaxAnimationTime_ = 0.7f;


	/// 感謝状を出す

	std::unique_ptr<Sprite> letter_;
	Vec3 letterPosition_;
	Vec2 letterSize_;

	bool isDrawLetter_ = false;

	float letterAnimationTime_ = 0.0f;
	float letterMaxAnimationTime_ = 1.0f;


};