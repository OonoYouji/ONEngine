#pragma once

/// std
#include <unordered_map>
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

	/// @brief rendering pipelineの取得
	/// @tparam T 取得する rendering pipeline の型
	/// @return 取得した rendering pipeline のポインタ
	template <class T>
	T* GetRenderingPipeline() requires std::is_base_of_v<IRenderingPipeline, T>;

	void DrawEntities();


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	ShaderCompiler*                   shaderCompiler_             = nullptr;
	DxManager*                        dxManager_                  = nullptr;
	class EntityCollection*           entityCollection_           = nullptr;
	class GraphicsResourceCollection* graphicsResourceCollection_ = nullptr;

	std::unordered_map<size_t, std::unique_ptr<IRenderingPipeline>> renderers_;

};



/// ===================================================
/// inline methods
/// ===================================================

template<class T, typename... Args>
inline void RenderingPipelineCollection::GenerateRenderingPipeline(Args&&... _args) requires std::is_base_of_v<IRenderingPipeline, T> {
	std::unique_ptr<T> renderer = std::make_unique<T>(std::forward<Args>(_args)...);
	renderer->Initialize(shaderCompiler_, dxManager_->GetDxDevice());
	renderers_.emplace(typeid(T).hash_code(), std::move(renderer));
}

template<class T>
inline T* RenderingPipelineCollection::GetRenderingPipeline() requires std::is_base_of_v<IRenderingPipeline, T> {
	return dynamic_cast<T*>(renderers_[typeid(T).hash_code()].get());
}
