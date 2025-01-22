#pragma once

/// std
#include <cmath>
#include <memory>
#include <vector>
#include <string>
#include <functional>

/// engine
#include "GraphicManager/PipelineState/PipelineState.h"
#include "GraphicManager/ModelManager/Model.h"

/// components
#include "ComponentManager/Base/BaseComponent.h"
#include "ComponentManager/Transform/Transform.h"


/// emitter, field
#include "ParticleEmitter.h"
#include "ParticleField.h"
#include <Math/Vector3.h>



/// ===================================================
/// パーティクルの発生、制御を行う
/// ===================================================
class ParticleSystem final : public BaseComponent {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	ParticleSystem(uint32_t maxParticleNum, const std::string& modelFilePath);
	~ParticleSystem();


	/// ===================================================
	/// public : static methods
	/// ===================================================

	static void SInitialize(ID3D12GraphicsCommandList* commandList);
	static void SFinalize();

	static void SetLightGroup(class LightGroup* _lightGroup);


	/// ===================================================
	/// public : overriding methods
	/// ===================================================

	void Initialize() override;
	void Update()     override;
	void Draw()       override;
	void Debug()      override;


	/// ===================================================
	/// public : non overriding methods
	/// ===================================================

	///// <summary>
	///// パーティクルの出現までの時間のセット
	///// </summary>
	///// <param name="_particleRespawnTime">: パーティクルの発生頻度</param>
	void SetParticleRespawnTime(float _particleRespawnTime);

	///// <summary>
	///// パーティクルの一回当たりの発生量のセット
	///// </summary>
	///// <param name="_emittedParticleCount">: 一回で何個パーティクルが出るか</param>
	void SetEmittedParticleCount(uint32_t _emittedParticleCount);

	/// <summary>
	/// パーティクルの寿命のセット
	/// </summary>
	/// <param name="_particleLifeTime">: パーティクルの寿命</param>
	void SetParticleLifeTime(float _particleLifeTime);

	/// <summary>
	/// ビルボードを使用するかセット
	/// </summary>
	/// <param name="_useBillboard">: ビルボードを使用するか</param>
	void SetUseBillboard(bool _useBillboard);

	/// <summary>
	/// パーティクルの更新処理関数をセット
	/// </summary>
	/// <param name="_function">: 更新処理関数 </param>
	void SetPartilceUpdateFunction(const std::function<void(class Particle*)>& _function);

	/// <summary>
	/// パーティクルのエミッターのフラグを設定
	/// </summary>
	/// <param name="particleEmitterFlags"></param>
	void SetParticleEmitterFlags(int particleEmitterFlags);

	/// <summary>
	/// バーストする瞬間にtrueを入れる
	/// </summary>
	/// <param name="_isBurst"></param>
	void SetBurst(bool _isBurst, float _burstTime, float _rateOverTime);

	/// <summary>
	/// エミッターの形状がボックスの min, maxを決める
	/// </summary>
	/// <param name="_min"></param>
	/// <param name="_max"></param>
	void SetBoxEmitterMinMax(const Vec3& _min, const Vec3& _max);

	void SetFieldArray(const std::list<ParticleField*>& _filedArray);

	void AddField(ParticleField* _field);


	/// <summary>
	/// const パーティクルの最大数のゲッタ
	/// </summary>
	/// <returns> return : パーティクルの最大数 </returns>
	uint32_t GetKMaxParticleNum() const {
		return kMaxParticleNum_;
	}

	/// <summary>
	/// パーティクル一個当たりの寿命のゲッタ
	/// </summary>
	/// <returns> return : パーティクルの寿命 </returns>
	float GetParticleLifeTime() const {
		return particleLifeTime_;
	}

	/// <summary>
	/// バースト中かどうかのフラグのゲッタ
	/// </summary>
	/// <returns></returns>
	bool GetIsBurst() const {
		return emitter_->isBurst_;
	}

private:

	/// ===================================================
	/// private : static objects
	/// ===================================================

	static std::unique_ptr<class ParticlePipeline> sPipeline_;


	/// ===================================================
	/// private : const objects
	/// ===================================================

	const uint32_t kMaxParticleNum_ = 128u;



	/// ===================================================
	/// private : objects
	/// ===================================================

	std::vector<std::unique_ptr<class Particle>> particleArray_;

	Model* pModel_ = nullptr;

	/// particle setting
	float particleLifeTime_; /// パーティクルの寿命
	bool  useBillboard_;     /// ビルボードを使用するか


	/// particles trasform buffers
	Microsoft::WRL::ComPtr<ID3D12Resource> trasformArrayBuffer_ = nullptr;
	Mat4*                                  mappingData_         = nullptr;
	D3D12_GPU_DESCRIPTOR_HANDLE            gpuHandle_;
	D3D12_CPU_DESCRIPTOR_HANDLE            cpuHandle_;
	uint32_t srvDescriptorIndex_;

	/// billboard
	Mat4 matBackToFront_;
	Mat4 matBillboard_;

	/// particle function
	std::function<void(Particle*)> particleUpdateFunc_;

	/// emitter
	std::unique_ptr<ParticleEmitter> emitter_ = nullptr;

	/// field
	std::list<ParticleField*> pFieldArray_;

};



/// ===================================================
/// パーティクルのパイプラインを管理する 
/// ===================================================
class ParticlePipeline final {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	ParticlePipeline() {}
	~ParticlePipeline() {}

	void Initialize(ID3D12GraphicsCommandList* commandList);
	void Update();

	void Draw(
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle,
		Model* useModel,
		uint32_t instanceCount
	);

	void SetLightGroup(class LightGroup* _lightGroup);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unique_ptr<PipelineState> pipelineState_ = nullptr;
	PipelineState::Shader          shader_;
	ID3D12GraphicsCommandList*     pCommandList_  = nullptr;
	class LightGroup*              pLightGroup_   = nullptr;

};



/// ===================================================
/// パーティクルの一粒子のクラス、byteが少なければ少ないほどいい
/// ===================================================
class Particle {
	friend class ParticlePipeline;
	friend class ParticleSystem;
	friend class ParticleEmitter;
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Particle() {}
	~Particle() {}

	void Initialize(); 
	void LifeTimeUpdate();

	bool GetIsAlive() const { return isAlive_; }

	const Transform& GetTransform() const { return transform_; }
	Transform* GetTransform() { return &transform_; }

	float GetNormLifeTime() { return lifeTime_ / maxLifeTime_; }

	uint32_t GetID() { return id_; }

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	uint32_t id_;

	bool isAlive_      = true;
	float lifeTime_    = 10.0f; // seconds
	float maxLifeTime_ = 10.0f; // seconds

	Transform transform_{};


};