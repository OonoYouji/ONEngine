#pragma once
#include <vector>
#include <list>
#include <string>

#include <GameObjectManager.h>
#include <ModelManager.h>
#include <Sprite.h>
//#include <Random.h>
#include "Random/Random.h"
#include <Transform.h>
#include "Particle/Grain.h"
#include "Particle/Grain2D.h"



class Effect : public BaseGameObject{
public:
	Effect() { CreateTag(this); }
	~Effect();

	void Initialize()override;

	void Reset();

	void Update()override;

	void Draw()override;
	void FrontSpriteDraw()override;

	void Debug()override;

	void Create();
	void Create2D();

	void EffectStart();
	void EffectStop();

	void SetGrainMode(int type);

	void SetOverType(int type);
	void OverTimeSetting(int rateTime, int appear);
	void OverDistanceSetting(float rateDistance);

	void ShapeType(int type);

	void ShiftingSpeedType(int type);
	void ShiftSpeedSetting(float shiftSpeed);

	void SizeChangeSetting(bool sizeChange, bool isReduction, bool isExpand, Vector3 endSize);

	void SetPos(const Vector2& pos);
	void SetVariavles2D(const Vector3& gravity, float speed, const Vector3& size, int lifeTime, bool sizeRandom, float minSizeRandom, float maxSizeRandom);

private:

	bool is3DMode_ = false;
	bool is2DMode_ = true;

	bool isStart_ = false;

	std::unique_ptr<Sprite> sprite_;
	std::string emitterName_ = "a";
	Model* model_ = nullptr;

	std::list<Grain*> grains_;
	std::list<Grain2D*> grain2Ds_;


	Transform transform_{};

	Vector3 gravity_ = { 0.0f,0.0f,0.0f };

	float speed_ = 0.05f;

	Vector3 position2D_ = { 0.0f,0.0f,0.0f };
	Vector3 rotation_ = { 0.0f,0.0f,0.0f };// 後で、rotateSpeedも追加する

	Vector3 size_ = { 5.0f,5.0f,1.0f };

	int lifeTime_ = 20;

	bool isRotateRandom_ = false;
	bool isSizeRandom = false;
	float minRotateRandom_ = -3.0f;
	float maxRotateRandom_ = 3.0f;
	float minSizeRandom_ = 4.0f;
	float maxSizeRandom_ = 6.0f;



	int provisional_ = 0;
	int secondProvisional_ = 0;
	int thirdProvisional_ = 0;
	int forceProvisional_ = 0;

	/// <summary>
	/// Rateの種類
	/// </summary>
	bool isOverTime_ = true;
	bool isOverDistance_ = false;

	// interval
	int rateTime_ = 4;
	int currentRateTime = rateTime_;
	int appear_ = 3;
	float rateDistance_ = 1.0f;
	float accumulationDistance = 0.0f;
	Vector3 previousPosition_;



	/// <summary>
	/// 形に沿った粒子の生成方向
	/// </summary>
	bool isCircle_ = true;
	bool isCone_ = false;
	bool isBox_ = false;

	// 生成方向のrandom制限用
	float xRandomLimite = 1.0f;
	float yRamdomLimite = 1.0f;
	float zRamdomLimite = 0.0f;

	float coneLenght_ = 1.0f;
	float coneAngle_ = 1.5f;
	float boxSizeX_ = 0.4f;
	float boxSizeZ_ = 0.4f;



	/// <summary>
	/// 発射後に速度が変わるのか
	/// </summary>
	bool isNormal_ = true;
	bool isDeceleration_ = false;
	bool isAccele_ = false;

	// どのくらい速度が変わるのか
	float shiftingSpeed_ = 0.0f;//0だったら止まらない、1だとすぐに止まる



	/// <summary>
	/// 時間で色が変わる
	/// </summary>
	bool isColorShift_ = false;

	// どの色からどの色へ変わっていくのか
	Vector4 originalColor_ = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 changeColor_ = { 0.0f,0.0f,0.0f,0.0f };



	/// <summary>
	/// 時間でサイズを変える
	/// </summary>
	bool isSizeChange_ = false;

	// どう変えるのか
	bool isReduction_ = true;
	bool isExpand_ = false;
	Vector3 endSize_ = { 0.1f,0.1f,0.1f };
};
