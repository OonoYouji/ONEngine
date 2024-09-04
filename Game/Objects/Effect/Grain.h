#pragma once
#include <ModelManager.h>
#include <Transform.h>


enum class ShiftSpeedType : uint32_t{
	kNormal,
	kDeceleration,
	kAccele,
};

enum class SizeChangeType :uint32_t {
	kReduction,
	kExpand,
};


class Grain
{
public:
	Grain();
	~Grain();

	void Initialze(Model* model, const Transform& trans, Vector3& gravity, const Vector3& velocity, const int life,
		const ShiftSpeedType shiftType, const float shiftSpeed, const bool colorShift, const Vector4& origin,
		const Vector4& change, const bool sizeChange, SizeChangeType chageType);

	void Update();

	void Draw();

	bool IsDead()const { return isDead_; }

private:

	Model* model_;
	Transform transform_{};
	Vector3 gravity_ = { 0.0f,0.0f,0.0f };
	Vector3 velocity_ = { 0.0f,1.0f,0.0f };
	// rotateSpeedを入れる
	int lifeTime_ = 10;
	bool isDead_ = false;

	/// <summary>
	/// 発射後に速度が変わるのか
	/// </summary>
	ShiftSpeedType speedType_ = ShiftSpeedType::kNormal;
	float shiftingSpeed_ = 0.0f;

	/// <summary>
	/// 時間で色が変わる
	/// </summary>
	bool isColorShift_ = false;
	Vector4 originColor_ = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 changeColor_ = { 0.0f,0.0f,0.0f,0.0f };

	/// <summary>
	/// 時間でサイズを変える
	/// </summary>
	bool isSizeChange_ = false;
	SizeChangeType changeType_ = SizeChangeType::kReduction;

};