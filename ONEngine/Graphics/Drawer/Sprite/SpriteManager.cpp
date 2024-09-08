#include <SpriteManager.h>

#include <WinApp.h>
#include <DxCommon.h>
#include <DxCommand.h>
#include <DxResourceCreator.h>
#include <DxDescriptor.h>


/// ===================================================
/// インスタンス確保
/// ===================================================
SpriteManager* SpriteManager::GetInstance() {
	static SpriteManager instance;
	return &instance;
}



/// ===================================================
/// 初期化
/// ===================================================
void SpriteManager::Initialize() {
	
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



	viewProjectionBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(Mat4));

	viewProjectionBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&matViewProjectionData_));

	Matrix4x4 projectionMatrix = Mat4::MakeOrthographicMatrix(0.0f, 0.0f, float(ONE::WinApp::kWindowSizeX), float(ONE::WinApp::kWindowSizeY), 0.0f, 1000.0f);
	*matViewProjectionData_ = Mat4::kIdentity * projectionMatrix;


}


/// ===================================================
/// 終了処理
/// ===================================================
void SpriteManager::Finalize() {
	pipelineState_.reset();
	viewProjectionBuffer_.Reset();
}



/// ===================================================
/// 描画前処理
/// ===================================================
void SpriteManager::PreDraw() {

	activeSprites_.clear();

}



/// ===================================================
/// 描画後処理
/// ===================================================
void SpriteManager::PostDraw() {
	if(activeSprites_.empty()) { return; }

	ID3D12GraphicsCommandList* commandList = ONE::DxCommon::GetInstance()->GetDxCommand()->GetList();
	
	pipelineState_->SetPipelineState();

	ONE::DxDescriptor* dxDescriptor = ONE::DxCommon::GetInstance()->GetDxDescriptor();
	dxDescriptor->SetSRVHeap(commandList);
	commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->SetGraphicsRootConstantBufferView(0, viewProjectionBuffer_->GetGPUVirtualAddress());

	for(auto& sprite : activeSprites_) {
		sprite->BindCBuffer(commandList);
		commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
	}

}



/// ===================================================
/// アクティブなスプライトの追加
/// ===================================================
void SpriteManager::AddActiveSprite(Sprite* sprite) {
	activeSprites_.push_back(sprite);
}
