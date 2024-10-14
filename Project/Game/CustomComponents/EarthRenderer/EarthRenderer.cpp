#include "EarthRenderer.h"

/// engine
#include "Core/ONEngine.h"
#include "GraphicManager/ModelManager/ModelManager.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDevice.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxResourceCreator.h"

/// objects
#include "Objects/Camera/Manager/BaseCamera.h"
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
		Vec4  color;
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

		void AddElement(const Vec4& position, const Vec4& color, float radius);

	private:
		/// ===================================================
		/// private : objects
		/// ===================================================

		/// pipeline
		std::unique_ptr<PipelineState> pipelineState_ = nullptr;
		PipelineState::Shader          shader_{};

		/// other class pointer
		ID3D12GraphicsCommandList* pCommandList_      = nullptr;
		ONE::DxDescriptor*         pDxDescriptor_     = nullptr;
		Transform*                 pTransform_        = nullptr;
		DirectionalLight*          pDirectionalLight_ = nullptr;

		/// buffer dest data
		uint32_t             maxEntityNum_;
		uint32_t             currentEntityNum_;
		std::vector<Element> elementArray_;
		Model*               model_      = nullptr;

		/// buffer resources
		ComPtr<ID3D12Resource>      elementArrayBuffer_ = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE elementArrayCpuHandle_;
		D3D12_GPU_DESCRIPTOR_HANDLE elementArrayGpuHandle_;

		ComPtr<ID3D12Resource>      elementSizeBuffer_   = nullptr;

		/// buffer mapping data
		int*     elementSizeMappingData_  = nullptr;
		Element* elementArrayMappingData_ = nullptr;

		/// varable...
		Mat4 matViewport_{
			640.0f,   0.0f,    0.0f,  640.5f,
			0.0f,   -360.0f,   0.0f,  360.5f,
			0.0f,     0.0f,    1.0f,    0.0f,
			0.0f,     0.0f,    0.0f,    1.0f
		};



	};


	/// ===================================================
	/// Earth Pipeline
	/// ===================================================

	EarthPipeline::EarthPipeline() {}
	EarthPipeline::~EarthPipeline() {}

	void EarthPipeline::Initialize(ID3D12GraphicsCommandList* commandList, ONE::DxDescriptor* dxDescriptor, uint32_t maxEntityNum) {

		/// ---------------------------------------------------
		/// variable setting
		/// ---------------------------------------------------

		/// other class pointer setting
		pCommandList_ = commandList;
		assert(pCommandList_);

		pDxDescriptor_ = dxDescriptor;
		assert(pDxDescriptor_);

		/// param setting
		maxEntityNum_ = maxEntityNum;
		model_        = ModelManager::Load("Sphere"); /// 地球用モデルが別にあるならそっちを読み込む


		/// ---------------------------------------------------
		/// pipeline create
		/// ---------------------------------------------------

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
		pipelineState_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 2);	///- element size

		/// Descriptor
		pipelineState_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
		pipelineState_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

		/// SRV - Texture
		pipelineState_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);
		pipelineState_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);

		/// SRV - Element
		pipelineState_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 1);


		/// Pipeline Create
		pipelineState_->Initialize();


		/// ---------------------------------------------------
		/// buffer create
		/// ---------------------------------------------------

		/// array - structured buffer
		elementArrayBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(Element) * maxEntityNum_);

		/// desc setting
		D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
		desc.Format                     = DXGI_FORMAT_UNKNOWN;
		desc.Shader4ComponentMapping    = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.ViewDimension              = D3D12_SRV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement        = 0;
		desc.Buffer.Flags               = D3D12_BUFFER_SRV_FLAG_NONE;
		desc.Buffer.NumElements         = static_cast<UINT>(maxEntityNum_);
		desc.Buffer.StructureByteStride = sizeof(Element);

		/// cpu, gpu handle initialize
		elementArrayCpuHandle_ = pDxDescriptor_->GetSrvCpuHandle();
		elementArrayGpuHandle_ = pDxDescriptor_->GetSrvGpuHandle();
		pDxDescriptor_->AddSrvUsedCount();

		/// resource create
		auto dxDevice = ONEngine::GetDxCommon()->GetDxDevice();
		dxDevice->GetDevice()->CreateShaderResourceView(elementArrayBuffer_.Get(), &desc, elementArrayCpuHandle_);


		/// size - constant buffer
		elementSizeBuffer_  = ONE::DxResourceCreator::CreateResource(sizeof(int));


		/// ---------------------------------------------------
		/// buffer mapping
		/// ---------------------------------------------------

		elementSizeBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&elementSizeMappingData_ ));
		elementArrayBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&elementArrayMappingData_));

		*elementSizeMappingData_ = 0;

	}

	void EarthPipeline::PreDraw() {
		/// 要素を削除
		elementArray_.clear();
	}

	void EarthPipeline::PostDraw() {

		if(elementArray_.empty()) {
			return;
		}

		/// 描画する
		pipelineState_->SetPipelineState();

		/// memcpy
		std::memcpy(elementArrayMappingData_, elementArray_.data(), sizeof(Mat4) * elementArray_.size());
		*elementSizeMappingData_ = static_cast<uint32_t>(elementArray_.size());

		/// get other class pointer
		CameraManager* cameraManager = CameraManager::GetInstance();
		ID3D12Resource* viewBuffer = cameraManager->GetMainCamera()->GetViewBuffer();

		/// default setting
		pDxDescriptor_->SetSRVHeap(pCommandList_);
		pipelineState_->SetPipelineState();
		pCommandList_->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		/// buffer setting
		pCommandList_->SetGraphicsRootConstantBufferView(0, viewBuffer->GetGPUVirtualAddress());
		pTransform_->BindTransform(pCommandList_, 1);
		pDirectionalLight_->BindToCommandList(3, pCommandList_);
		pCommandList_->SetGraphicsRootConstantBufferView(4, elementSizeBuffer_->GetGPUVirtualAddress());
		pCommandList_->SetGraphicsRootDescriptorTable(6, elementArrayGpuHandle_);

		/// buffer setting  and  draw call
		model_->DrawCall(pCommandList_, nullptr, 2u, 5u);

	}


	void EarthPipeline::AddElement(const Vec4& position, const Vec4& color, float radius) {

		/// 上限に達していないかチェック
		if(maxEntityNum_ <= elementArray_.size()) { 
			return;
		}

		/// 要素を追加
		Element add{ .position = position, .color = color, .radius = radius };
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
	color_    = Vec4::kRed;
	radius_   = 10.0f;
}

void EarthRenderer::Update() {

	const Vec3& ownerPosition = GetOwner()->GetPosition();
	position_ = Vec4(ownerPosition.x, ownerPosition.y, ownerPosition.z, 1.0f);
}

void EarthRenderer::Draw() {
	gPipeline->AddElement(position_, color_, radius_);
}


void EarthRenderer::SetRadius(float _radius) {
	radius_ = _radius;
}

void EarthRenderer::SetColor(const Vec4& _color) {
	color_ = _color;
}
