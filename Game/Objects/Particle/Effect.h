#pragma once
#include <vector>
#include <list>
#include <string>


#include <ModelManager.h>
//#include <Random.h>
#include "Random/Random.h"
#include <Transform.h>
#include "Particle/Grain.h"



class Effect {
public:
	Effect();
	~Effect();

	void Initialize(const std::string& name);

	void Reset();

	void Update();

	void Draw();

	void Setting();

	void Create();



private:

	std::string emitterName_ = "a";
	Model* model_ = nullptr;

	std::list<Grain*> grains_;


	Transform transform_{};

	Vector3 gravity_ = { 0.0f,0.0f,0.0f };

	float speed_ = 0.05f;

	Vector3 rotation_ = { 0.0f,0.0f,0.0f };// 後で、rotateSpeedも追加する

	Vector3 size_ = { 0.1f,0.1f,0.1f };

	int lifeTime_ = 20;

	bool isRotateRandom_ = false;
	bool isSizeRandom = false;
	float minRotateRandom_ = -3.0f;
	float maxRotateRandom_ = 3.0f;
	float minSizeRandom_ = 0.01f;
	float maxSizeRandom_ = 2.0f;



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
	float rateDistance_ = 4.0f;
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
	float coneAngle_ = 2.0f;
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
