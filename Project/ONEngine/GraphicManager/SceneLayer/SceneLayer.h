#pragma once

/// std
#include <string>

#include "GraphicManager/RenderTextureManager/RenderTexture.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommand.h"

#include "GraphicManager/PostEffect/PostEffectProcessor/PostEffectProcessor.h"


/// ===================================================
/// シーンの描画レイヤー
/// ===================================================
class SceneLayer final {
public:
	
	/// ===================================================
	/// public : methods
	/// ===================================================

	SceneLayer();
	~SceneLayer() {}

	static void ResetInstanceCount();

	void Initialize(const std::string& className, class BaseCamera* camera);

	void Draw();

protected:

	/// ===================================================
	/// protected : objects
	/// ===================================================

	static int                     sInstanceCount_;

	std::string                    className_;
	int                            id_;

	class BaseCamera*              camera_        = nullptr;
	class BaseScene*               currentScene_  = nullptr;
	std::unique_ptr<RenderTexture> renderTexture_ = nullptr;

	ONE::DxCommand*                pDxCommand_    = nullptr;

	std::unique_ptr<PostEffectProcessor> postEffectProcessor_    = nullptr;

public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	void SetMainCamera(class BaseCamera* _camera);

	RenderTexture* GetRenderTexture() { return renderTexture_.get(); }

	const std::string& GetName() { return className_; }
};