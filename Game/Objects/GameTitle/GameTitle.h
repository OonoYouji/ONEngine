#pragma once

#include <GameObjectManager.h>
#include <Sprite.h>

class GameTitle final : public BaseGameObject {
public:

	GameTitle() { CreateTag(this); }
	~GameTitle() {}

	void Initialize() override;
	void Update() override;
	void FrontSpriteDraw() override;

	void Debug() override;


	/// non overriding methods
	
	void SettingCameraAnimation();
	
	void CreateStartUI();

	void GameStart();

private:

	float animationTime_ = 0.0f;
	float animationSpeed_ = 1.0f;
	bool isAnimationStarted_ = false;

	bool isCreatedStartUI_ = false;
	bool isGameStart_ = false;


	std::unique_ptr<Sprite> title_;
	Vec3 titlePosition_;
	Vec2 titleSize_;
	Vec4 titleColor_;

	/// sin(ON); の文字がつぶれる動作のため
	class GameStartUI* pGameStartUI_ = nullptr;
	float startEffectAnimationTime_ = 0.0f;
	float startEffectMaxAnimationTime_ = 0.6f;
	bool isCreartedWave_ = false;

};