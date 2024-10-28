#include "Floor.h"

#include <Core/ONEngine.h>

#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommand.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDescriptor.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxResourceCreator.h"

#include "Objects/Camera/Manager/CameraManager.h"

#include "Math/Matrix3x3.h"


Floor::Floor() {}
Floor::~Floor() {}


/// ===================================================
/// 初期化
/// ===================================================
void Floor::Initialize() {

	pipeline_.reset(new PipelineState());
	shader_.ShaderCompile(
		L"Floor/Floor.VS.hlsl", L"vs_6_0",
		L"Floor/Floor.PS.hlsl", L"ps_6_0"
	);

	pipeline_->SetFillMode(kSolid);

	pipeline_->SetShader(&shader_);

	pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	pipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	pipeline_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	pipeline_->AddInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);

	pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0); /// view projection
	pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 1); /// transform
	pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0);  /// uvTransform

	pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);	/// texture range
	pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);		/// texture table
	pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);			/// sampler

	pipeline_->Initialize();

	transform_.Initialize();
	transform_.scale = Vec3(100, 1, 100);
	transform_.UpdateMatrix();

	material_.SetFilePath("Floor.png");
	material_.SetTextureName("Floor");
	material_.CreateBuffer();


	mesh_.AddVertex({ { -0.5f, 0.0f, -0.5f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } });
	mesh_.AddVertex({ { -0.5f, 0.0f,  0.5f, 1.0f }, { 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } });
	mesh_.AddVertex({ {  0.5f, 0.0f,  0.5f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } });
	mesh_.AddVertex({ {  0.5f, 0.0f, -0.5f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } });

	mesh_.AddIndex(0);
	mesh_.AddIndex(1);
	mesh_.AddIndex(2);
	mesh_.AddIndex(0);
	mesh_.AddIndex(2);
	mesh_.AddIndex(3);

	mesh_.CreateBuffer();

	uvTransformBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(Mat4));
	uvTransformBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&matUvTransform_));

	scale_ = Vec2(100, 100);
	Matrix3x3 matrix = Matrix3x3::MakeAffine(scale_, rotate_, translate_);
	*matUvTransform_ = matrix.ToMat4();

}


/// ===================================================
/// 描画
/// ===================================================
void Floor::Draw() {
	ID3D12GraphicsCommandList* commandList = ONEngine::GetDxCommon()->GetDxCommand()->GetList();
	ID3D12Resource* viewBuffer = CameraManager::GetInstance()->GetMainCamera()->GetViewBuffer();

	pipeline_->SetPipelineState();

	commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList->SetGraphicsRootConstantBufferView(0, viewBuffer->GetGPUVirtualAddress());
	transform_.BindTransform(commandList, 1);
	commandList->SetGraphicsRootConstantBufferView(2, uvTransformBuffer_->GetGPUVirtualAddress());
	material_.BindTexture(commandList, 3);

	mesh_.Draw(commandList, true);

}


