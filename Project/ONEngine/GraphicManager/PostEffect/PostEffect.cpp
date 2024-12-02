#include "PostEffect.h"

/// engine
#include "Core/ONEngine.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommand.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxBarrierCreator.h"
#include "GraphicManager/SceneLayer/SceneLayer.h"

#include "GraphicManager/TextureManager/TextureManager.h"


PostEffect::PostEffect() {}
PostEffect::~PostEffect() {}

void PostEffect::Initialize() {
	intermediateTextures_[0] = TextureManager::GetInstance()->CreateUAVTexture("intermediateTexture1", { 1280.0f, 720.0f }, DXGI_FORMAT_R8G8B8A8_UNORM);
	intermediateTextures_[1] = TextureManager::GetInstance()->CreateUAVTexture("intermediateTexture2", { 1280.0f, 720.0f }, DXGI_FORMAT_R8G8B8A8_UNORM);
}

void PostEffect::Execution() {

	/// post effectが空なら return
	if(postEffectPipelines_.empty()) {
		return;
	}

	size_t input = 0;



	/// ---------------------------------------------------
	/// RTV resourceのcopy
	/// ---------------------------------------------------

	ONE::DxBarrierCreator::CreateBarrier(
		intermediateTextures_[input]->GetResource(),
		intermediateTextures_[input]->GetCurrentState(),
		D3D12_RESOURCE_STATE_COPY_DEST
	);
	intermediateTextures_[input]->SetCurrentState(D3D12_RESOURCE_STATE_COPY_DEST);

	ONE::DxBarrierCreator::CreateBarrier(
		pLayerRenderTexture_->GetRenderTexResource(),
		pLayerRenderTexture_->currentResourceState,
		D3D12_RESOURCE_STATE_COPY_SOURCE
	);
	pLayerRenderTexture_->currentResourceState = D3D12_RESOURCE_STATE_COPY_SOURCE;


	/// resourceをcopy
	ID3D12GraphicsCommandList* pCommandList = ONEngine::GetDxCommon()->GetDxCommand()->GetList();
	pCommandList->CopyResource(
		intermediateTextures_[input]->GetResource(),
		pLayerRenderTexture_->GetRenderTexResource()
	);



	ONE::DxBarrierCreator::CreateBarrier(
		intermediateTextures_[input]->GetResource(),
		intermediateTextures_[input]->GetCurrentState(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);
	intermediateTextures_[input]->SetCurrentState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	ONE::DxBarrierCreator::CreateBarrier(
		pLayerRenderTexture_->GetRenderTexResource(),
		pLayerRenderTexture_->currentResourceState,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
	pLayerRenderTexture_->currentResourceState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;






	/// ---------------------------------------------------
	/// post effect execution
	/// ---------------------------------------------------

	for(auto& pipeline : postEffectPipelines_) {
		pipeline->Execution(
			intermediateTextures_[input],
			intermediateTextures_[1 - input]
		);

		/// inputを反転させる
		input = input ? 0 : 1;
	}




	/// ---------------------------------------------------
	/// RTV resourceのcopy
	/// ---------------------------------------------------

	ONE::DxBarrierCreator::CreateBarrier(
		intermediateTextures_[input]->GetResource(),
		intermediateTextures_[input]->GetCurrentState(),
		D3D12_RESOURCE_STATE_COPY_SOURCE
	);
	intermediateTextures_[input]->SetCurrentState(D3D12_RESOURCE_STATE_COPY_SOURCE);

	ONE::DxBarrierCreator::CreateBarrier(
		pLayerRenderTexture_->GetRenderTexResource(),
		pLayerRenderTexture_->currentResourceState,
		D3D12_RESOURCE_STATE_COPY_DEST
	);
	pLayerRenderTexture_->currentResourceState = D3D12_RESOURCE_STATE_COPY_DEST;


	/// resourceをcopy
	pCommandList->CopyResource(
		pLayerRenderTexture_->GetRenderTexResource(),
		intermediateTextures_[input]->GetResource()
	);



	ONE::DxBarrierCreator::CreateBarrier(
		intermediateTextures_[input]->GetResource(),
		intermediateTextures_[input]->GetCurrentState(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);
	intermediateTextures_[input]->SetCurrentState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	ONE::DxBarrierCreator::CreateBarrier(
		pLayerRenderTexture_->GetRenderTexResource(),
		pLayerRenderTexture_->currentResourceState,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
	pLayerRenderTexture_->currentResourceState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

}

void PostEffect::AddPipeline(BasePostEffectPipeline* _postEffectPipeline) {
	postEffectPipelines_.push_back(_postEffectPipeline);
}

void PostEffect::SetLayerRenderTexture(RenderTexture* _renderTexture) {
	pLayerRenderTexture_ = _renderTexture;
}
