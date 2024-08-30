#include <Particle.h>

#include <DxCommon.h>
#include <DxCommand.h>
#include <DxResourceCreator.h>
#include <DxDescriptor.h>

#include <CameraManager.h>
#include <TextureManager.h>
#include <Mesh.h>
#include <Random.h>


/// ===================================================
/// static member initialize
/// ===================================================
std::unique_ptr<PipelineState> Particle::sPipeline_ = nullptr;
PipelineState::Shader Particle::sShader_ = {};


/// ===================================================
/// 静的なメンバの初期化
/// ===================================================
void Particle::StaticInitialize() {

	sPipeline_.reset(new PipelineState);

	sShader_.ShaderCompile(
		L"Particle/Particle.VS.hlsl", L"vs_6_0",
		L"Particle/Particle.PS.hlsl", L"ps_6_0"
	);

	sPipeline_->SetFillMode(kSolid);

	sPipeline_->SetShader(&sShader_);
	sPipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	sPipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	sPipeline_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	sPipeline_->AddInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);

	sPipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);	/// viewProjection

	sPipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	sPipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	sPipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

	sPipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX, 0);	/// transform
	sPipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 1);	/// material
	sPipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 2);	/// texture

	sPipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);		/// sampler

	sPipeline_->Initialize();

}


/// ===================================================
/// 静的なメンバの初期化
/// ===================================================
void Particle::StaticFinalize() {
	sPipeline_.reset();
}





/// ===================================================
/// 初期化
/// ===================================================
void Particle::Initialize() {
	CreateTransformBuffer();
	CreateMaterialBuffer();
	CreateVertexBuffer();

	maxLifeTime_ = 10.0f;

	for(int i = 0; i < 10; i++) {
		CreateParticleElement();
	}

}


/// ===================================================
/// 更新処理
/// ===================================================
void Particle::Update() {

	const float deltaTime = 1.0f / 60.0f;

	for(auto& elem : elements_) {
		ParticleElementUpdate(&elem);
	}

	elements_.remove_if([](const ParticleElement& element) {
		return element.currentLifeTime <= 0.0f;
	});

}


/// ===================================================
/// 描画
/// ===================================================
void Particle::Draw() {

	/// 粒子が1つもなければ以降の処理を無視
	if(elements_.empty()) { return; }

	/// エレメントの情報を代入
	int id = 0;
	for(auto& elem : elements_) {

		elem.transform.UpdateMatrix(false);
		transformDatas_[id] = elem.transform.matTransform;
		materialDatas_[id] = *elem.material.GetData();

		id++;

		/// 最大数を超えたらエラーを吐かないようにbreak
		if(id > kMaxInstanceCount_) {
			break;
		}
	}


	ID3D12GraphicsCommandList* commandList = ONE::DxCommon::GetInstance()->GetDxCommand()->GetList();

	/// 使用するパイプラインの設定をする
	sPipeline_->SetPipelineState();

	ONE::DxCommon::GetInstance()->GetDxDescriptor()->SetSRVHeap(commandList);
	commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/// カメラの設定
	ID3D12Resource* viewBuffer = CameraManager::GetInstance()->GetMainCamera()->GetViewBuffer();
	commandList->SetGraphicsRootConstantBufferView(0, viewBuffer->GetGPUVirtualAddress());

	commandList->IASetVertexBuffers(0, 1, &vbv_);
	commandList->SetGraphicsRootDescriptorTable(1, transformGPUHandle_);
	commandList->SetGraphicsRootDescriptorTable(2, materialGPUHandle_);
	commandList->SetGraphicsRootDescriptorTable(3, TextureManager::GetInstance()->GetTexture("uvChecker").GetGPUHandle());

	commandList->DrawInstanced(3, static_cast<UINT>(elements_.size()), 0, 0);
}


/// ===================================================
/// particle element を新しく作成する
/// ===================================================
void Particle::CreateParticleElement() {

	ParticleElement element;
	element.material.Create();
	element.transform.position = Random::Vec3(-Vec3::kOne, Vec3::kOne);
	element.transform.UpdateMatrix(false);

	element.material.SetColor(Vec4(Random::Vec3(-Vec3::kOne, Vec3::kOne), 1.0f));

	element.velocity = Random::Vec3(-Vec3::kOne, Vec3::kOne);
	element.currentLifeTime = maxLifeTime_;

	elements_.push_back(std::move(element));

}



#pragma region private methods


/// ===================================================
/// 行列バッファの生成
/// ===================================================
void Particle::CreateTransformBuffer() {

	transformBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(Mat4) * kMaxInstanceCount_);

	transformBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&transformDatas_));

	for(int i = 0; i < kMaxInstanceCount_; ++i) {
		transformDatas_[i] = Mat4::kIdentity;
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	desc.Buffer.NumElements = kMaxInstanceCount_;
	desc.Buffer.StructureByteStride = sizeof(Transform::matTransform);


	ONE::DxDescriptor* dxDescriptor = ONE::DxCommon::GetInstance()->GetDxDescriptor();

	D3D12_CPU_DESCRIPTOR_HANDLE transformCPUHandle = dxDescriptor->GetSrvCpuHandle();
	transformGPUHandle_ = dxDescriptor->GetSrvGpuHandle();
	dxDescriptor->AddSrvUsedCount();

	ONE::DxCommon::GetInstance()->GetDevice()->CreateShaderResourceView(transformBuffer_.Get(), &desc, transformCPUHandle);


}



/// ===================================================
/// マテリアルバッファの生成
/// ===================================================
void Particle::CreateMaterialBuffer() {

	materialBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(Material::MaterialData) * kMaxInstanceCount_);

	materialBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&materialDatas_));

	for(int i = 0; i < kMaxInstanceCount_; ++i) {
		materialDatas_[i].color = Vec4::k1111;
		materialDatas_[i].uvTransform = Mat4::kIdentity;
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	desc.Buffer.NumElements = kMaxInstanceCount_;
	desc.Buffer.StructureByteStride = sizeof(Material::MaterialData);


	ONE::DxDescriptor* dxDescriptor = ONE::DxCommon::GetInstance()->GetDxDescriptor();

	D3D12_CPU_DESCRIPTOR_HANDLE materialCPUHandle = dxDescriptor->GetSrvCpuHandle();
	materialGPUHandle_ = dxDescriptor->GetSrvGpuHandle();
	dxDescriptor->AddSrvUsedCount();

	ONE::DxCommon::GetInstance()->GetDevice()->CreateShaderResourceView(materialBuffer_.Get(), &desc, materialCPUHandle);


}


/// ===================================================
/// 頂点バッファの生成
/// ===================================================
void Particle::CreateVertexBuffer() {

	vertexBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(Mesh::VertexData) * 3);

	Mesh::VertexData* vertexData;
	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

	vertexData[0] = { { 0.0f, 0.5f, 0.0f, 1.0f }, { 0.5f, 0.0f }, { 0.0f, 0.0f, -1.0f} };
	vertexData[1] = { { 0.5f, -0.5f, 0.0f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, -1.0f} };
	vertexData[2] = { { -0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f} };


	vbv_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vbv_.SizeInBytes = static_cast<UINT>(sizeof(Mesh::VertexData) * 3);
	vbv_.StrideInBytes = static_cast<UINT>(sizeof(Mesh::VertexData));

}



/// ===================================================
/// 粒子の更新処理
/// ===================================================
void Particle::ParticleElementUpdate(ParticleElement* elem) {

	/// TODO:   class Time::deltaTime のように他クラスから deltaTimeを得る構造にする
	const float deltaTime = 1.0f / 60.0f;

	elem->transform.position += elem->velocity * deltaTime;
	elem->currentLifeTime -= deltaTime;
}


#pragma endregion // private methods

