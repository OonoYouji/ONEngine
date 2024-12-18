#include "SpriteManager.h"


#include <Core/ONEngine.h>

#include "WindowManager/WinApp.h"

#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommand.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxResourceCreator.h"

#include "GraphicManager/GraphicsEngine/DirectX12/DxDescriptorHeap.h"

#include "Objects/Camera/Manager/CameraManager.h"


using namespace ONE;

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
	pipelineState_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL,  0);	///- material

	pipelineState_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	pipelineState_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);
	pipelineState_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);

	pipelineState_->Initialize();


}


/// ===================================================
/// 終了処理
/// ===================================================
void SpriteManager::Finalize() {
	pipelineState_.reset();
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

	ID3D12GraphicsCommandList* commandList = ONEngine::GetDxCommon()->GetDxCommand()->GetList();
	ID3D12Resource* viewBuffer = CameraManager::GetInstance()->GetMainCamera()->GetViewBuffer();

	pipelineState_->SetPipelineState();

	commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->SetGraphicsRootConstantBufferView(0, viewBuffer->GetGPUVirtualAddress());

	for(auto& sprite : activeSprites_) {
		sprite.pSprite_->BindCBuffer(commandList);
		commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
	}

}


/// ===================================================
/// アクティブなスプライトの追加
/// ===================================================
void SpriteManager::AddActiveSprite(Sprite* sprite) {
	activeSprites_.push_back(ActiveSprite(sprite));
}
