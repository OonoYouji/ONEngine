#pragma once

/// std
#include <cstdint>
#include <vector>
#include <memory>
#include <functional>

/// math
#include "Math/Vector3.h"


/// ===================================================
/// enum
/// ===================================================

enum class EMISSION_TYEP  : int32_t {
	TIME, DISTANCE
};

enum class EMISSION_SHAPE : int32_t {
	BOX, SPHERE
};

enum PARTICLE_EMITTER_FLAGS {
	PARTICLE_EMITTER_NONE   = 1 << 0,
	PARTICLE_EMITTER_NOTIME = 1 << 1,
	PARTICLE_EMITTER_BURST  = 1 << 2,
};


/// ===================================================
/// パーティクルの出現範囲のクラス
/// ===================================================
class ParticleEmitter {
	friend class ParticleSystem;
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ParticleEmitter() {}
	~ParticleEmitter() {}

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="_particleArray">  : particle system :: particle array へのポインタ </param>
	/// <param name="_particleSystem"> : particle system へのポインタ                   </param>
	void Initialize(
		std::vector<std::unique_ptr<class Particle>>* _particleArray,
		class ParticleSystem* _particleSystem
	);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(std::function<void(class Particle*)> _particleStartupFunc);

	/// <summary>
	/// imguiを使ってデバッグ表示
	/// </summary>
	void Debug();



	/// <summary>
	/// パーティクルの発生
	/// </summary>
	void Emit(std::function<void(class Particle*)> _particleStartupFunc);


	/// <summary>
	/// バーストしたいタイミングでtrueにする
	/// </summary>
	/// <param name="_isBurst"></param>
	void SetBurst(bool _isBurst, float _burstTime, float _rateOverTime);

	/// <summary>
	/// エミッターの設定
	/// </summary>
	/// <param name="_particleEmitterFlags"></param>
	void SetParticleEmitterFlags(int _particleEmitterFlags);


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// particle emitter
	class BaseGameObject* owner_ = nullptr;
	Vec3  min_, max_; /// emission shape == box    -> boxの中に出現
	float radius_;    /// emission shape == sphere -> sphereの中に出現

	int32_t emissionType_;  /// 出現方法
	int32_t emissionShape_; /// 形状
	int32_t particleEmitterFlags_;

	/// particle emission time
	float rateOverTime_;	 /// emission type == time     -> 時間経過で出現
	float rateOverDistance_; /// emission type == distance -> 一定距離移動で出現

	float currentTime_;
	float currentDistance_;


	/// particle emission count
	uint32_t emissionCount_;        /// 出現数
	uint32_t currentParticleCount_; /// 出現している数
	uint32_t maxParticleCount_;

	/// particle emission burst
	bool  isBurst_; /// 使用例 : 敵に当たった瞬間にonにしてパーティクルを出現させる
	float burstTime_;
	float burstRateOverTime_;
	float maxBurstRateOverTime_;

	/// other pointer
	std::vector<std::unique_ptr<class Particle>>* pParticleArray_  = nullptr;
	class ParticleSystem*                         pParticleSystem_ = nullptr;

};