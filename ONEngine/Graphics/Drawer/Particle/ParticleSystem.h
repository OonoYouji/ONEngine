#pragma once

#include <cmath>
#include <memory>
#include <vector>

#include <PipelineState/PipelineState.h>
#include <Component/Base/BaseComponent.h>



/// ===================================================
/// パーティクルの発生、制御を行う
/// ===================================================
class ParticleSystem final : public BaseComponent {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	ParticleSystem(uint32_t maxParticleNum);
	~ParticleSystem() {}


	/// ===================================================
	/// public : static methods
	/// ===================================================

	static void SInitialize();
	static void SFinalize();


	/// ===================================================
	/// public : overriding methods
	/// ===================================================

	void Initialize() override;
	void Update()     override;



	/// ===================================================
	/// public : non overriding methods
	/// ===================================================

	void Draw();



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

	void Initialize();
	void Update();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unique_ptr<PipelineState> pipelineState_ = nullptr;
	PipelineState::Shader          shader_;
	
};



/// ===================================================
/// パーティクルの一粒子のクラス、byteが少なければ少ないほどいい
/// ===================================================
class Particle final {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Particle() {}
	~Particle() {}

	void Initialize(); 
	void Update();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	bool isAlive_ = true;

};