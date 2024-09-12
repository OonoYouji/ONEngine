#pragma once

#include <GameObjectManager.h>
#include <Sprite.h>

class GameResult final : public BaseGameObject {

	enum EFFECT_PAHSE : uint32_t {
		SOSEI_BEGIN_PLAYAUDIO,	/// 1, sosei_beginをながす
		HEART_BERAK,			/// 2, 心臓が膨張→破裂(パーティクルを出す) 
		CAMERA_MOVING,			/// 3, カメラをタイトルのスタート地点に移動させる
		KILLED_ENEMIES_LEAVE,	/// 4.1, 倒した分の敵が落ちてくる (溜める)
		KILLED_ENEMIES_DROPING,	/// 4.2, 倒した分の敵が落ちてくる (落とす)
		EFFECT_END,
		PAHSE_WAIT,
		EFFECT_PAHSE_COUNTER
	};

	struct DropData {
		std::unique_ptr<Sprite> sprite;
		Vec3 endPosition;
		Vec3 startPosition;
		float time;
		bool isStart;
	};

	struct TexData {
		std::string texName, filePath;
	};


public:

	GameResult() { CreateTag(this); }
	~GameResult() {}

	void Initialize() override;
	void Update() override;

	void BackSpriteDraw() override;
	void FrontSpriteDraw() override;


	/// non overriding methods

	void HeartBreak();

	void CameraMoving();

	void KilledEnemiesLeave();
	void KilledEnemiesDropping();

	void EffectEndUpdate();

	void WaitTime(EFFECT_PAHSE nextPahse, float waitTime);

private:


	uint32_t effectPhase_ = 0u;
	uint32_t nextPahse_ = 0u;
	float waitTime_ = 0.0f;

	class Wave* pWave_ = nullptr;
	float waveAnimationTime_ = 0.0f;
	float waveMaxAnimationTime_ = 1.0f;
	float waveAnimationSpeed_ = 20.0f;


	/// 1, sosei_beginを流す
	class AudioSource* sosei_begin_ = nullptr;

	/// 2, 心臓が膨張 → 破裂(パーティクルを出す)
	class Heart* heart_ = nullptr;
	class Hand* hand_ = nullptr;
	class HeartBottom* heartBottom_ = nullptr;

	float heartBreakAnimationTime_ = 0.0f;

	class Effect* heartBreakEffect_ = nullptr;
	uint32_t heartBreakEffectRemainingFrame_ = 12u;	/// パーティクルが出る残り時間
	bool isBreaked_ = false;

	/// 3, カメラをタイトルのスタート地点に移動させる
	float cameraMovingTime_ = 0.0f;
	float cameraMovingMaxTime_ = 0.0f;


	/// 4, 倒した分の敵が落ちてくる
	uint32_t killedEnemiesCount_ = 0u;
	uint32_t largeEnemySpriteNum_ = 0u;
	
	std::vector<DropData> dropDatas_;

	float droppingMaxAnimationTime_ = 1.0f;
	float waveAlphaLerpTime_ = 0.5f;

	/// 5, スコアが増えていく(数字)、モニター全体が埋まっていい
	bool isDrawScore_ = false;
	uint32_t totalScore_ = 0u;
	std::vector<std::unique_ptr<Sprite>> digitNumbers_;
	
	std::vector<TexData> texDatas_;

};