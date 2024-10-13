#pragma once

/// std
#include <cmath>
#include <memory>
#include <vector>
#include <string>
#include <functional>

/// engine
#include "GraphicManager/GraphicsEngine/DirectX12/DxDescriptor.h"
#include "GraphicManager/PipelineState/PipelineState.h"
#include "GraphicManager/ModelManager/Model.h"

/// components
#include "ComponentManager/Base/BaseComponent.h"
#include "ComponentManager/Transform/Transform.h"



/// ===================================================
/// パーティクルの発生、制御を行う
/// ===================================================
class ParticleSystem final : public BaseComponent {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	ParticleSystem(uint32_t maxParticleNum, const std::string& modelFilePath);
	~ParticleSystem() {}


	/// ===================================================
	/// public : static methods
	/// ===================================================

	static void SInitialize(ID3D12GraphicsCommandList* commandList, ONE::DxDescriptor* dxDescriptor);
	static void SFinalize();


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

	/// <summary>
	/// パーティクルの出現までの時間のセット
	/// </summary>
	/// <param name="_particleRespawnTime">: パーティクルの発生頻度</param>
	void SetParticleRespawnTime(float _particleRespawnTime);

	/// <summary>
	/// パーティクルの一回当たりの発生量のセット
	/// </summary>
	/// <param name="_emittedParticleCount">: 一回で何個パーティクルが出るか</param>
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
	/// パーティクルを発生させる
	/// </summary>
	void EmitParticles();

private:

	/// ===================================================
	/// private : static objects
	/// ===================================================

	static std::unique_ptr<class ParticlePipeline> gPipeline;


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
	uint32_t particleInstanceCount_;      /// 現在のパーティクル発生数
	uint32_t emittedParticleCount_;       /// 一回の発生で出現するパーティクルの数
	float    particleRespawnTime_;        /// パーティクルが発生するまでの時間
	float    currentParticleRespawnTime_; /// 現在のリスポーンタイム
	float    particleLifeTime_;           /// パーティクルの寿命
	bool     useBillboard_;               /// ビルボードを使用するか


	/// particles trasform buffers
	Microsoft::WRL::ComPtr<ID3D12Resource> trasformArrayBuffer_ = nullptr;
	Mat4*                                  mappingData_         = nullptr;
	D3D12_GPU_DESCRIPTOR_HANDLE            gpuHandle_;
	D3D12_CPU_DESCRIPTOR_HANDLE            cpuHandle_;


	/// billboard
	Mat4 matBackToFront_;
	Mat4 matBillboard_;

	/// particle function
	std::function<void(Particle*)> particleUpdateFunc_;

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

	void Initialize(ID3D12GraphicsCommandList* commandList, ONE::DxDescriptor* dxDescriptor);
	void Update();

	void Draw(
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle,
		Model* useModel,
		uint32_t instanceCount
	);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unique_ptr<PipelineState> pipelineState_ = nullptr;
	PipelineState::Shader          shader_;
	ID3D12GraphicsCommandList*     pCommandList_  = nullptr;
	ONE::DxDescriptor*             pDxDescriptor_ = nullptr;
	
};



/// ===================================================
/// パーティクルの一粒子のクラス、byteが少なければ少ないほどいい
/// ===================================================
class Particle {
	friend class ParticlePipeline;
	friend class ParticleSystem;
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

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	bool isAlive_      = true;
	float lifeTime_    = 10.0f; // seconds
	float maxLifeTime_ = 10.0f; // seconds

	Transform transform_{};


};