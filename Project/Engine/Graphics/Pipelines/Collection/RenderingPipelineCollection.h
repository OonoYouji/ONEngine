#pragma once

/// std
#include <vector>
#include <memory>

/// engine
#include "../Interface/IRenderingPipeline.h"
#include "Engine/Graphics/Shader/ShaderCompiler.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"

/// ===================================================
/// renderer collection
/// ===================================================
class RenderingPipelineCollection final {
public:
	
	/// ===================================================
	/// public : methods
	/// ===================================================

	RenderingPipelineCollection(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager, class EntityCollection* _entityCollection, class GraphicsResourceCollection* _graphicsResourceCollection);
	~RenderingPipelineCollection();

	/// @brief 初期化関数
	void Initialize();

	/// @brief rendering pipelineの生成
	/// @tparam T 生成する rendering pipelineの型
	template <class T, typename... Args>
	void GenerateRenderingPipeline(Args&&... _args) requires std::is_base_of_v<IRenderingPipeline, T>;

	/// @brief すべてのEntityを描画する
	void DrawEntities();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	ShaderCompiler*                   shaderCompiler_             = nullptr;
	DxManager*                        dxManager_                  = nullptr;
	class EntityCollection*           entityCollection_           = nullptr;
	class GraphicsResourceCollection* graphicsResourceCollection_ = nullptr;

	std::vector<std::unique_ptr<IRenderingPipeline>> renderers_;

};



/// ===================================================
/// inline methods
/// ===================================================

template<class T, typename... Args>
inline void RenderingPipelineCollection::GenerateRenderingPipeline(Args&&... _args) requires std::is_base_of_v<IRenderingPipeline, T> {
	std::unique_ptr<T> renderer = std::make_unique<T>(std::forward<Args>(_args)...);
	renderer->Initialize(shaderCompiler_, dxManager_);
	renderers_.push_back(std::move(renderer));
}
