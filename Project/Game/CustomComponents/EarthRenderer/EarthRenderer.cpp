#include "EarthRenderer.h"

/// objects
#include "GameObjectManager/BaseGameObject.h"




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
	public:
		/// ===================================================
		/// public : methods
		/// ===================================================

		EarthPipeline();
		~EarthPipeline();

		void Initialize(ID3D12GraphicsCommandList* commandList, uint32_t maxEntityNum);

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
		uint32_t maxEntityNum_;
		std::vector<Element> elementArray_;

		/// other class pointer
		ID3D12GraphicsCommandList* pCommandList_ = nullptr;

	};


	/// ===================================================
	/// Earth Pipeline
	/// ===================================================

	EarthPipeline::EarthPipeline() {}
	EarthPipeline::~EarthPipeline() {}

	void EarthPipeline::Initialize(ID3D12GraphicsCommandList* commandList, uint32_t maxEntityNum) {

		/// other class pointer setting
		pCommandList_ = commandList;
		assert(pCommandList_);

		/// param setting
		maxEntityNum_ = maxEntityNum;


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
		pipelineState_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 1);	///- tranform
		pipelineState_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);	///- viewProjection
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

void EarthRenderer::SInitialize(ID3D12GraphicsCommandList* commandList, uint32_t maxEntityNum) {
	gPipeline.reset(new EarthPipeline);
	gPipeline->Initialize(commandList, maxEntityNum);
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
