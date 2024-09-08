#pragma once

#include <GameObjectManager.h>


class Heart final : public BaseGameObject {
public:

	Heart() { CreateTag(this); }
	~Heart() {}

	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	class HeartBottom* bottom_ = nullptr;
	class HeartAbove* above_ = nullptr;
};


class HeartBottom final : public BaseGameObject {
public:
	HeartBottom() { CreateTag(this); }
	~HeartBottom() {}

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Debug() override;
private:
	float animationTime_ = 0.0f;
	float speed_ = 8.0f;
	float amplitude_ = 0.05f;
	Model* model_ = nullptr;
};


class HeartAbove final : public BaseGameObject {
public:
	HeartAbove() { CreateTag(this); }
	~HeartAbove() {}

	void Initialize() override;
	void Update() override;
	void Draw() override;
private:
	Model* model_ = nullptr;
};