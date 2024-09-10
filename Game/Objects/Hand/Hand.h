#pragma once

#include <GameObjectManager.h>

class Hand final : public BaseGameObject {
public:

	Hand() { CreateTag(this); }
	~Hand() {}

	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	//Model* model_;
	class HandFinger* finger_ = nullptr;
	class HandPalm* palm_ = nullptr;
	class HandThumb* thumb_ = nullptr;
};


/// <summary>
/// 指
/// </summary>
class HandFinger final : public BaseGameObject {
public:
	HandFinger() { CreateTag(this); }
	~HandFinger() {}
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Debug() override;
private:
	float animationTime_ = 0.0f;
	float speed_ = 1.0f;
	float amplitude_ = 1.0f;
	Model* model_ = nullptr;
	/// other class pointer
	class SinWaveDrawer* pSinWaveDrawer_ = nullptr;
};


/// <summary>
/// 親指
/// </summary>
class HandThumb final : public BaseGameObject {
public:
	HandThumb() { CreateTag(this); }
	~HandThumb() {}
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Debug() override;
private:
	float animationTime_ = 0.0f;
	float speed_ = 1.0f;
	float amplitude_ = 1.0f;
	Model* model_ = nullptr;
	/// other class pointer
	class SinWaveDrawer* pSinWaveDrawer_ = nullptr;
};



/// <summary>
/// 手のひら
/// </summary>
class HandPalm final : public BaseGameObject {
public:
	HandPalm() { CreateTag(this); }
	~HandPalm() {}
	void Initialize() override;
	void Update() override;
	void Draw() override;
private:
	Model* model_ = nullptr;
};

