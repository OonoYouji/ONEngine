#pragma once

/// std
#include <vector>
#include <memory>

/// engine
#include "../Pipelines/Collection/RenderingPipelineCollection.h"
#include "../Shader/ShaderCompiler.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h" 
#include "Engine/Core/Window/WindowManager.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"
#include "Engine/Graphics/RenderTexture/SceneRenderTexture.h"
#include "Engine/Graphics/Pipelines/Render/CopyImage/CopyImageRenderingPipeline.h"


/// ===================================================
/// 描画のフレームワーククラス
/// ===================================================
class RenderingFramework final {

	enum RENDER_TEXTURE_TYPE {
		RENDER_TEXTURE_SCENE,  ///< シーンの描画用
		RENDER_TEXTURE_DEBUG,  ///< デバッグ用
		RENDER_TEXTURE_PREFAB, ///< Prefabの描画用
		RENDER_TEXTURE_COUNT,  ///< レンダーテクスチャの数
	};

public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	RenderingFramework();
	~RenderingFramework();

	/// @brief 初期化
	void Initialize(DxManager* _dxManager, WindowManager* _windowManager, class EntityComponentSystem* _pEntityComponentSystem);

	/// @brief 作成された順番に描画を行う
	void Draw();


	void DrawScene();
	void DrawDebug();
	void DrawPrefab();


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unique_ptr<ShaderCompiler>              shaderCompiler_;
	std::unique_ptr<RenderingPipelineCollection> renderingPipelineCollection_;
	std::unique_ptr<GraphicsResourceCollection>  resourceCollection_;
	std::vector<std::unique_ptr<SceneRenderTexture>> renderTextures_;
	DxManager*                                   dxManager_        = nullptr;
	WindowManager*                               windowManager_    = nullptr;
	class EntityComponentSystem*                 pEntityComponentSystem_ = nullptr;


#ifdef _DEBUG
	class ImGuiManager*                          imGuiManager_ = nullptr;
#else

	std::unique_ptr<CopyImageRenderingPipeline> copyImagePipeline_;
	Window* releaseBuildSubWindow_;

#endif // _DEBUG

	//std::unique_ptr<CopyImageRenderingPipeline> copyImagePipeline_;

public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// @brief 描画に使用するリソースコレクションの取得
	/// @return リソースのコレクションを返す
	GraphicsResourceCollection* GetResourceCollection() const { return resourceCollection_.get(); }



#ifdef _DEBUG
	/// @brief ImGuiManagerへのポインタを設定
	/// @param _imGuiManager ImGuiManagerへのポインタ
	void SetImGuiManager(class ImGuiManager* _imGuiManager) { imGuiManager_ = _imGuiManager; }
#endif // _DEBUG

};

