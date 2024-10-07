#pragma once

#include <cmath>
#include <memory>
#include <vector>
#include <string>

#include "GraphicManager/GraphicsEngine/DirectX12/DxDescriptor.h"

#include "GraphicManager/PipelineState/PipelineState.h"
#include "ComponentManager/Base/BaseComponent.h"
#include "ComponentManager/Transform/Transform.h"

#include "GraphicManager/ModelManager/Model.h"


/// ===================================================
/// パーティクルの発生、制御を行う
/// ===================================================
class ParticleSystem final : public BaseComponent {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	ParticleSystem(uint32_t maxParticleNum, const std::string& filePath);
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
		const std::vector<std::unique_ptr<class Particle>>& particleArray, 
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
class Particle final {
	friend class ParticlePipeline;
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Particle() {}
	~Particle() {}

	void Initialize(); 
	void Update();

	bool GetIsAlive() const { return isAlive_; }

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	bool isAlive_ = true;

	Transform transform_{};

};