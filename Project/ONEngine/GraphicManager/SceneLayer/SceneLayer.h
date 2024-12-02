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

	/// <summary>
	/// インスタンスIDのリセット
	/// </summary>
	static void ResetInstanceCount();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="className"></param>
	/// <param name="camera"></param>
	void Initialize(const std::string& _className, class BaseCamera* _camera);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

protected:

	/// ===================================================
	/// protected : objects
	/// ===================================================

	static int                           sInstanceCount_;

	std::string                          className_;
	int                                  id_;

	class BaseCamera*                    camera_                 = nullptr;
	class BaseScene*                     currentScene_           = nullptr;
	std::unique_ptr<RenderTexture>       renderTexture_          = nullptr;

	ONE::DxCommand*                      pDxCommand_             = nullptr;

	std::unique_ptr<PostEffectProcessor> postEffectProcessor_    = nullptr;

public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// <summary>
	/// メインカメラのセット
	/// </summary>
	/// <param name="_camera"> : メインカメラのポインタ </param>
	void SetMainCamera(class BaseCamera* _camera);

	/// <summary>
	/// RTVTextureのゲット
	/// </summary>
	/// <returns></returns>
	RenderTexture* GetRenderTexture() { return renderTexture_.get(); }

	/// <summary>
	/// クラス名のゲッタ
	/// </summary>
	/// <returns></returns>
	const std::string& GetName() { return className_; }

	/// <summary>
	/// ポストエフェクトのパイプラインのセット
	/// </summary>
	/// <param name="_postEffectPipeline"> : post effect pipeline </param>
	void AddPostEffectPipeline(BasePostEffectPipeline* _postEffectPipeline);
};