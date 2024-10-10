#include "EarthRenderer.h"

/// engine
#include "Core/ONEngine.h"
#include "GraphicManager/ModelManager/ModelManager.h"

/// objects
#include "Objects/Camera/Manager/CameraManager.h"
#include "GameObjectManager/BaseGameObject.h"
#include "GraphicManager/Light/DirectionalLight.h"


/// ===================================================
/// unsigned namespace
/// ===================================================
namespace {

	/// ===================================================
	/// 1エンティティから必要な値
	/// ===================================================
	struct Element {
		Vec4  position;
		float radius;
	};


	/// ===================================================
	/// 地球を描画するためのパイプライン
	/// ===================================================
	class EarthPipeline {
		friend class EarthRenderer;
	public:
		/// ===================================================
		/// public : methods
		/// ===================================================

		EarthPipeline();
		~EarthPipeline();

		void Initialize(ID3D12GraphicsCommandList* pCommandList_, ONE::DxDescriptor* dxDescriptor, uint32_t maxEntityNum);

		void PreDraw();
		void PostDraw();

		void AddElement(const Vec4& position, float radius);

	private:
		/// ===================================================
		/// private : objects
		/// ===================================================

		/// pipeline
		std::unique_ptr<PipelineState> pipelineState_ = nullptr;
		PipelineState::Shader          shader_{};

		/// draw data
		uint32_t             maxEntityNum_;
		std::vector<Element> elementArray_;
		Model*               model_      = nullptr;

		/// other class pointer
		ID3D12GraphicsCommandList* pCommandList_      = nullptr;
		ONE::DxDescriptor*         pDxDescriptor_     = nullptr;
		Transform*                 pTransform_        = nullptr;
		DirectionalLight*          pDirectionalLight_ = nullptr;

	};


	/// ===================================================
	/// Earth Pipeline
	/// ===================================================

	EarthPipeline::EarthPipeline() {}
	EarthPipeline::~EarthPipeline() {}

	void EarthPipeline::Initialize(ID3D12GraphicsCommandList* commandList, ONE::DxDescriptor* dxDescriptor, uint32_t maxEntityNum) {

		/// other class pointer setting
		pCommandList_ = commandList;
		assert(pCommandList_);

		pDxDescriptor_ = dxDescriptor;
		assert(pDxDescriptor_);

		/// param setting
		maxEntityNum_ = maxEntityNum;
		model_        = ModelManager::Load("Sphere"); /// 地球用モデルが別にあるならそっちを読み込む


		/// pipeline create
		shader_.ShaderCompile(
			L"../CustomShaders/Earth/Earth.VS.hlsl", L"vs_6_0",
			L"../CustomShaders/Earth/Earth.PS.hlsl", L"ps_6_0"
		);

		pipelineState_.reset(new PipelineState);

		/// Shader, FillMode, Topology
		pipelineState_->SetShader(&shader_);
		pipelineState_->SetFillMode(kSolid);
		pipelineState_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

		/// Input Layout
		pipelineState_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		pipelineState_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
		pipelineState_->AddInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);

		/// Constant Buffer
		pipelineState_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);	///- viewProjection
		pipelineState_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 1);	///- tranform
		pipelineState_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0);	///- material
		pipelineState_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 1);	///- directional light

		/// Descriptor
		pipelineState_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

		/// SRV - Texture
		pipelineState_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);
		pipelineState_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);

		/// Pipeline Create
		pipelineState_->Initialize();


	}

	void EarthPipeline::PreDraw() {
		/// 要素を削除
		elementArray_.clear();
	}

	void EarthPipeline::PostDraw() {
		/// 描画する
		pipelineState_->SetPipelineState();


		CameraManager* cameraManager = CameraManager::GetInstance();
		ID3D12Resource* viewBuffer = cameraManager->GetMainCamera()->GetViewBuffer();

		pDxDescriptor_->SetSRVHeap(pCommandList_);
		pipelineState_->SetPipelineState();

		pCommandList_->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pCommandList_->SetGraphicsRootConstantBufferView(0, viewBuffer->GetGPUVirtualAddress());
		pDirectionalLight_->BindToCommandList(3, pCommandList_);

		pTransform_->BindTransform(pCommandList_, 1);
		model_->DrawCall(pCommandList_, nullptr, 2u, 4u);

	}


	void EarthPipeline::AddElement(const Vec4& position, float radius) {

		/// 上限に達していないかチェック
		if(maxEntityNum_ >= elementArray_.size()) { return; }

		/// 要素を追加
		Element add{ .position = position, .radius = radius };
		elementArray_.push_back(add);
	}


	/// object initializing
	std::unique_ptr<EarthPipeline> gPipeline;

}
/// ===================================================
/// unsignet namespace :: EarthPipeline
/// ===================================================




/// ===================================================
/// Earth Renderer
/// ===================================================


EarthRenderer::EarthRenderer() {}
EarthRenderer::~EarthRenderer() {}


/// ===================================================
/// public : static methods
/// ===================================================

void EarthRenderer::SInitialize(ID3D12GraphicsCommandList* commandList, ONE::DxDescriptor* dxDescriptor, uint32_t maxEntityNum) {
	gPipeline.reset(new EarthPipeline);
	gPipeline->Initialize(commandList, dxDescriptor, maxEntityNum);
}

void EarthRenderer::SFinalize() {
	gPipeline.reset();
}


void EarthRenderer::PreDraw() {
	gPipeline->PreDraw();
}

void EarthRenderer::PostDraw() {
	gPipeline->PostDraw();
}

void EarthRenderer::SetEarthTransform(Transform* _transform) {
	gPipeline->pTransform_ = _transform;
}

void EarthRenderer::SetDirectionalLight(DirectionalLight* _directionalLight) {
	gPipeline->pDirectionalLight_ = _directionalLight;
}


/// ===================================================
/// public : methods
/// ===================================================

void EarthRenderer::Initialize() {
	position_ = { 0, 0, 0, 1 };
	radius_ = 10.0f;
}

void EarthRenderer::Update() {

	const Vec3& ownerPosition = GetOwner()->GetTransform()->position;
	position_ = Vec4(ownerPosition.x, ownerPosition.y, ownerPosition.z, 1.0f);

}

void EarthRenderer::Draw() {
	gPipeline->AddElement(position_, radius_);
}

void EarthRenderer::SetRadius(float _radius) {
	radius_ = _radius;
}
