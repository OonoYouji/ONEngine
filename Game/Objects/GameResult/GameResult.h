#pragma once

#include <GameObjectManager.h>
#include <Sprite.h>

class GameResult final : public BaseGameObject {
public:

	GameResult() { CreateTag(this); }
	~GameResult() {}

	void Initialize() override;
	void Update() override;

	void BackSpriteDraw() override;

	//void Debug() override;

	void WaveUpdate();

private:

	class Wave* pWave_ = nullptr;
	float waveAnimationTime_ = 0.0f;
	float waveMaxAnimationTime_ = 1.0f;
	float waveAnimationSpeed_ = 20.0f;


	/// 1, sosei_beginを流す
	class AudioSource* sosei_begin_ = nullptr;

	/// 2, 心臓が膨張 → 破裂(パーティクルを出す)


	/// 3, カメラをタイトルのスタート地点に移動させる

	/// 4, 倒した分の敵が落ちてくる

	/// 5, スコアが増えていく(数字)、モニター全体が埋まっていい

};