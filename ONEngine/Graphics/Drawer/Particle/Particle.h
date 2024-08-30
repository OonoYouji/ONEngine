#pragma once

#include <wrl/client.h>
#include <d3d12.h>

#include <memory>
#include <list>

#include <PipelineState.h>
#include <Material.h>
#include <Transform.h>


using namespace Microsoft::WRL;

/// =========================================================
/// パーティクルの一粒子
/// =========================================================
struct ParticleElement {
	Transform transform;
	Material material;

	Vec3 velocity = { 0, 1, 0 };
	float currentLifeTime;
};


/// =========================================================
/// パーティクル
/// =========================================================
class Particle final {
public:

	Particle() {}
	~Particle() {}


	/// =========================================================
	/// public : static methods
	/// =========================================================

	/// <summary>
	/// staticメンバの初期化
	/// </summary>
	static void StaticInitialize();

	/// <summary>
	/// staticメンバの終了
	/// </summary>
	static void StaticFinalize();

	
	
	
	/// =========================================================
	/// public : methods
	/// =========================================================

	void Initialize();

	void Update();

	void Draw();
	

	void CreateParticleElement();


private:

	/// =========================================================
	/// private : methods
	/// =========================================================

	void CreateTransformBuffer();

	void CreateMaterialBuffer();

	void CreateVertexBuffer();


	void ParticleElementUpdate(ParticleElement* elem);

private:

	/// =========================================================
	/// private : static objects
	/// =========================================================

	static std::unique_ptr<PipelineState> sPipeline_;
	static PipelineState::Shader sShader_;

	static constexpr int kMaxInstanceCount_ = 128;
	

	
	/// =========================================================
	/// private : objects
	/// =========================================================

	ComPtr<ID3D12Resource> transformBuffer_ = nullptr;
	Mat4* transformDatas_ = nullptr;	/// mapping用データ
	D3D12_GPU_DESCRIPTOR_HANDLE transformGPUHandle_;

	ComPtr<ID3D12Resource> materialBuffer_ = nullptr;
	Material::MaterialData* materialDatas_ = nullptr;	/// mapping用データ
	D3D12_GPU_DESCRIPTOR_HANDLE materialGPUHandle_;


	ComPtr<ID3D12Resource> vertexBuffer_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vbv_{};

	std::list<ParticleElement> elements_;
	
	float maxLifeTime_; /// particle currentTimeの初期化時間 : 単位は秒

};