#include "SceneRenderTexture.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Graphics/Shader/GraphicsPipeline.h"

SceneRenderTexture::SceneRenderTexture() {
	name_ = "";
	clearColor_ = Vector4(0.1f, 0.25f, 0.5f, 1.0f);
}


void SceneRenderTexture::Initialize( 
	const std::string& _name, const Vector4& _clearColor, const Vector2& _textureSize,
	DxManager* _dxm, AssetCollection* _assetCollection) {

	/// パラメータの設定
	name_ = _name;
	clearColor_ = _clearColor;

	/// 新規のDepthStencilを作成
	pDxDepthStencil_ = _dxm->AddDepthStencil(_name);


	/// texture init

	{	/// game render textures
		renderTextures_.resize(4);
		for (auto& renderTexture : renderTextures_) {
			renderTexture = std::make_unique<RenderTexture>();
		}

		renderTextures_[0]->Initialize(static_cast<DXGI_FORMAT>(RTVFormat::Color),         clearColor_, _textureSize, name_ + "Scene",         _dxm, pDxDepthStencil_, _assetCollection);
		renderTextures_[1]->Initialize(static_cast<DXGI_FORMAT>(RTVFormat::WorldPosition), clearColor_, _textureSize, name_ + "WorldPosition", _dxm, pDxDepthStencil_, _assetCollection);
		renderTextures_[2]->Initialize(static_cast<DXGI_FORMAT>(RTVFormat::Normal),        clearColor_, _textureSize, name_ + "Normal",        _dxm, pDxDepthStencil_, _assetCollection);
		renderTextures_[3]->Initialize(static_cast<DXGI_FORMAT>(RTVFormat::Flags),         {},          _textureSize, name_ + "Flags",         _dxm, pDxDepthStencil_, _assetCollection);
	}

}

void SceneRenderTexture::SetRenderTarget(DxCommand* _dxCommand, DxDSVHeap* _dxDSVHeap) {
	renderTextures_[0]->SetRenderTarget(
		_dxCommand, _dxDSVHeap,
		renderTextures_
	);
}

void SceneRenderTexture::CreateBarrierRenderTarget(DxCommand* _dxCommand) {
	for (auto& renderTexture : renderTextures_) {
		renderTexture->CreateBarrierRenderTarget(_dxCommand);
	}
}

void SceneRenderTexture::CreateBarrierPixelShaderResource(DxCommand* _dxCommand) {
	for (auto& renderTexture : renderTextures_) {
		renderTexture->CreateBarrierPixelShaderResource(_dxCommand);
	}
}

const std::string& SceneRenderTexture::GetName(size_t _index) const {
	if (_index < renderTextures_.size()) {
		return renderTextures_[_index]->GetName();
	}

	static const std::string emptyString = "";
	return emptyString;
}

const std::string& SceneRenderTexture::GetName() const {
	return name_;
}

