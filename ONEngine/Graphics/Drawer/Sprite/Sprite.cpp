#include <Sprite.h>

#include <WinApp.h>
#include <DxCommon.h>
#include <DxCommand.h>
#include <DxResourceCreator.h>

#include <Model.h>




namespace {

	Mat4 MakeOrthographicMatrix(float l, float t, float r, float b, float nearZ, float farZ) {
		return Mat4{
			{ 2.0f / (r - l), 0.0f, 0.0f, 0.0f},
			{ 0.0f, 2.0f / (t - b), 0.0f, 0.0f},
			{ 0.0f, 0.0f, 1.0f / (farZ - nearZ), 0.0f},
			{ (l + r) / (l - r), (t + b) / (b - t), nearZ / (nearZ - farZ), 1.0f}
		};
	}

}





/// ===================================================
/// static objects intialize
/// ===================================================
std::unique_ptr<PipelineState> Sprite::pipelineState_ = nullptr;
PipelineState::Shader Sprite::shader_;


Sprite::Sprite() {}
Sprite::~Sprite() {}


/// ===================================================
/// staticメンバ変数の初期化
/// ===================================================
void Sprite::StaticInitialize() {

	pipelineState_.reset(new PipelineState());

	shader_.ShaderCompile(
		L"Sprite/Sprite.VS.hlsl", L"vs_6_0",
		L"Sprite/Sprite.PS.hlsl", L"ps_6_0"
	);


	pipelineState_->SetShader(&shader_);
	pipelineState_->SetFillMode(kSolid);

	pipelineState_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	pipelineState_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	pipelineState_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

	pipelineState_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);	///- viewProjection
	pipelineState_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 1);	///- transform
	pipelineState_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0);	///- material

	pipelineState_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	pipelineState_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);
	pipelineState_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);

	pipelineState_->Initialize();

}



/// ===================================================
/// static変数の終了処理
/// ===================================================
void Sprite::StaticFinalize() {
	pipelineState_.reset();
}



/// ===================================================
/// 初期化
/// ===================================================
void Sprite::Initialize(const std::string& textureName, const std::string& filePath) {


	/// ---------------------------------------------------
	/// メッシュの初期化
	/// ---------------------------------------------------
	CreateVertexBuffer();


	/// ---------------------------------------------------
	/// マテリアルの初期化
	/// ---------------------------------------------------
	material_.SetFilePath(filePath);
	material_.SetTextureName(textureName);

	material_.Create();


	/// ---------------------------------------------------
	/// 定数バッファの初期化
	/// ---------------------------------------------------
	CreateConstantBuffer();


}



/// ===================================================
/// 描画
/// ===================================================
void Sprite::Draw() {
	ID3D12GraphicsCommandList* commandList = ONE::DxCommon::GetInstance()->GetDxCommand()->GetList();

	pipelineState_->SetPipelineState();

	commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &vbv_);
	commandList->IASetIndexBuffer(&ibv_);


	commandList->SetGraphicsRootConstantBufferView(0, viewProjectionBuffer_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, transformBuffer_->GetGPUVirtualAddress());
	material_.BindMaterial(commandList, 2);
	material_.BindTexture(commandList, 3);

	commandList->DrawIndexedInstanced(static_cast<UINT>(indices_.size()), 1, 0, 0, 0);
}


/// ===================================================
/// 頂点バッファの生成
/// ===================================================
void Sprite::CreateVertexBuffer() {

	vertices_.push_back({ { -1.0f, -1.0f, 0.0f, 1.0f}, { 0.0f, 1.0f } }); ///-  左下
	vertices_.push_back({ { -1.0f,  1.0f, 0.0f, 1.0f}, { 0.0f, 0.0f } }); ///-  左上
	vertices_.push_back({ {  1.0f,  1.0f, 0.0f, 1.0f}, { 1.0f, 0.0f } }); ///-  右上
	vertices_.push_back({ {  1.0f, -1.0f, 0.0f, 1.0f}, { 1.0f, 1.0f } }); ///-  右下

	indices_.push_back(0);
	indices_.push_back(1);
	indices_.push_back(2);
	indices_.push_back(0);
	indices_.push_back(2);
	indices_.push_back(3);


	/// ---------------------------------------------------
	/// 頂点
	/// ---------------------------------------------------
	vertexBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(VertexPosUv) * vertices_.size());

	VertexPosUv* vertexData = nullptr;
	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, vertices_.data(), sizeof(VertexPosUv) * vertices_.size());

	vbv_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vbv_.SizeInBytes = static_cast<UINT>(sizeof(VertexPosUv) * vertices_.size());
	vbv_.StrideInBytes = static_cast<UINT>(sizeof(VertexPosUv));


	/// ---------------------------------------------------
	/// インデックス
	/// ---------------------------------------------------

	indexBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(uint32_t) * indices_.size());

	uint32_t* indexData = nullptr;
	indexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	std::memcpy(indexData, indices_.data(), sizeof(uint32_t) * indices_.size());

	ibv_.BufferLocation = indexBuffer_->GetGPUVirtualAddress();
	ibv_.Format = DXGI_FORMAT_R32_UINT;
	ibv_.SizeInBytes = static_cast<UINT>(sizeof(uint32_t) * indices_.size());

}


/// ===================================================
/// 定数バッファの生成
/// ===================================================
void Sprite::CreateConstantBuffer() {

	transformBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(TransformData));

	transformBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&transformData_));
	transformData_->matWorld = Mat4::MakeScale(Vec3(100, 100, 1.0f)) * Mat4::MakeTranslate(Vec3(200, -200, 0));

	viewProjectionBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(ViewProjectionData));

	viewProjectionBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&viewProjectionData_));

	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, float(ONE::WinApp::kWindowSizeX), float(-ONE::WinApp::kWindowSizeY), 0.0f, 100.0f);
	viewProjectionData_->matVp = Mat4::kIdentity * projectionMatrix;

}




