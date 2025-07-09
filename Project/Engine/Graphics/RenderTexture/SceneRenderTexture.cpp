#include "SceneRenderTexture.h"

SceneRenderTexture::SceneRenderTexture() {
	name_ = "";
	clearColor_ = Vector4(0.1f, 0.25f, 0.5f, 1.0f);
}


void SceneRenderTexture::Initialize(
	const std::string& _name, const Vector4& _clearColor,
	DxManager* _dxManager, GraphicsResourceCollection* _graphicsResourceCollection) {

	/// パラメータの設定
	name_ = _name;
	clearColor_ = _clearColor;


	/// texture init

	{	/// game render textures
		renderTextures_.resize(4);
		for (auto& renderTexture : renderTextures_) {
			renderTexture = std::make_unique<RenderTexture>();
		}

		renderTextures_[0]->Initialize(DXGI_FORMAT_R8G8B8A8_UNORM,     clearColor_, name_ + "Scene",         _dxManager, _graphicsResourceCollection);
		renderTextures_[1]->Initialize(DXGI_FORMAT_R16G16B16A16_FLOAT, clearColor_, name_ + "WorldPosition", _dxManager, _graphicsResourceCollection);
		renderTextures_[2]->Initialize(DXGI_FORMAT_R16G16B16A16_FLOAT, clearColor_, name_ + "Normal",        _dxManager, _graphicsResourceCollection);
		renderTextures_[3]->Initialize(DXGI_FORMAT_R8G8B8A8_UNORM,     {},          name_ + "Flags",         _dxManager, _graphicsResourceCollection);
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

