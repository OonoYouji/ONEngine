#include "RenderTexture.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"

void RenderTexture::Initialize(const Vector4& _clearColor, DxManager* _dxManager) {
	clearColor_ = _clearColor;

	
	/// create instance
	texture_ = std::make_unique<Texture>();
	//texture_->Initialize(_dxManager->GetDxDevice(), 1920, 1080, DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

	DxDevice* dxDevice = _dxManager->GetDxDevice();
	DxRTVHeap* dxRTVHeap = _dxManager->GetDxRTVHeap();
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

	/// render texture resourceの作成
	renderTextureResource_.CreateRenderTextureResource(
		dxDevice, Vector2(1280.0f, 720.0f), format, _clearColor
	);

	uint32_t rtvHeapIndex = dxRTVHeap->Allocate();


}


void RenderTexture::SetRenderTarget(DxCommand* _dxCommand) {
	auto command = _dxCommand->GetCommandList();
	//command->OMSetRenderTargets
}
