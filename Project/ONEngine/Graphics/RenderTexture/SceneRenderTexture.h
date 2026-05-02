#pragma once

/// std
#include <vector>
#include <memory>
#include <string>

/// engine
#include "RenderTexture.h"

namespace ONEngine {
class DxManager;
class DxCommand;
class DxDSVHeap;
class DxDepthStencil;
}

namespace ONEngine::Asset {
class AssetCollection;
}



/// @brief シーンのレンダリングテクスチャの種類
enum SCENE_RTV {
	SCENE_RTV_COLOR,
	SCENE_RTV_WORLD_POSITION,
	SCENE_RTV_NORMAL,
	SCENE_RTV_FLAGS,
};

/// ///////////////////////////////////////////////////
/// シーンのレンダリングテクスチャ
/// ///////////////////////////////////////////////////
namespace ONEngine {

class SceneRenderTexture final {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	SceneRenderTexture();
	~SceneRenderTexture() = default;

	void Initialize(
		const std::string& _name, const Vector4& _clearColor, const Vector2& _textureSize,
		DxManager* _dxm, Asset::AssetCollection* _assetCollection
	);


	void SetRenderTarget(DxCommand* _dxCommand, DxDSVHeap* _dxDSVHeap);

	void CreateBarrierRenderTarget(DxCommand* _dxCommand);
	void CreateBarrierPixelShaderResource(DxCommand* _dxCommand);

	const std::string& GetName(size_t _index) const;
	const std::string& GetName() const;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::string name_;
	Vector4 clearColor_;

	std::vector<std::unique_ptr<RenderTexture>> renderTextures_;
	DxDepthStencil* pDxDepthStencil_ = nullptr;

};


} /// ONEngine
