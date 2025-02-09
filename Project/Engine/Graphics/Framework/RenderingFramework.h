#pragma once

/// std
#include <vector>
#include <memory>

/// engine
#include "../Pipelines/Collection/RenderingPipelineCollection.h"
#include "../Shader/ShaderCompiler.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h" 
#include "Engine/Core/Window/WindowManager.h"

/* TODO: RenderingFrameworkクラスの実装
 *
 * ここにRendererのリストを持たせる
 * RendererはIRendererを継承している
 *
*/


/// ===================================================
/// 描画のフレームワーククラス
/// ===================================================
class RenderingFramework final {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	RenderingFramework(class EntityCollection* _entityCollection);
	~RenderingFramework();

	/// @brief 初期化
	void Initialize(DxManager* _dxManager, WindowManager* _windowManager);

	/// @brief 作成された順番に描画を行う
	void Draw();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unique_ptr<ShaderCompiler>              shaderCompiler_;
	std::unique_ptr<RenderingPipelineCollection> renderingPipelineCollection_;

	DxManager*                      dxManager_        = nullptr;
	WindowManager*                  windowManager_    = nullptr;
	class EntityCollection*         entityCollection_ = nullptr;

};

