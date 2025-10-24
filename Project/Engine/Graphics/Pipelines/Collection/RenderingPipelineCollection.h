#pragma once

/// std
#include <vector>
#include <memory>

/// engine
#include "../Interface/IRenderingPipeline.h"
#include "../Interface/IPostProcessPipeline.h"

/// ///////////////////////////////////////////////////
/// renderer collection
/// ///////////////////////////////////////////////////
class RenderingPipelineCollection final {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	RenderingPipelineCollection(ShaderCompiler* _shaderCompiler, class DxManager* _dxm, class EntityComponentSystem* _pEntityComponentSystem, class AssetCollection* _assetCollection);
	~RenderingPipelineCollection();

	/// @brief 初期化関数
	void Initialize();



	/// @brief rendering pipelineの生成
	/// @tparam T 生成する rendering pipelineの型
	template <class T, typename... Args>
	void Generate3DRenderingPipeline(Args&&... _args) requires std::is_base_of_v<IRenderingPipeline, T>;

	template <class T, typename... Args>
	void Generate2DRenderingPipeline(Args&&... _args) requires std::is_base_of_v<IRenderingPipeline, T>;

	template <class T, typename... Args>
	void GeneratePostProcessPipeline(Args&&... _args) requires std::is_base_of_v<IPostProcessPipeline, T>;



	/// @brief すべてのEntityを描画する
	void PreDrawEntities(class CameraComponent* _3dCamera, class CameraComponent* _2dCamera);
	void DrawEntities(class CameraComponent* _3dCamera, class CameraComponent* _2dCamera);

	void DrawSelectedPrefab(class CameraComponent* _3dCamera, class CameraComponent* _2dCamera);

	/// @brief post processの実行
	void ExecutePostProcess(const std::string& _sceneTextureName);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	class DxManager*             pDxManager_;
	class EntityComponentSystem* pEntityComponentSystem_;
	class AssetCollection*       pAssetCollection_;
	class ShaderCompiler*        pShaderCompiler_;

	std::unique_ptr<IRenderingPipeline> prefabRenderingPipeline_;

	std::vector<std::unique_ptr<IRenderingPipeline>>   renderer3ds_;
	std::vector<std::unique_ptr<IRenderingPipeline>>   renderer2ds_;
	std::vector<std::unique_ptr<IPostProcessPipeline>> postProcesses_;
};



/// ===================================================
/// inline methods
/// ===================================================

template<class T, typename... Args>
inline void RenderingPipelineCollection::Generate3DRenderingPipeline(Args&&... _args) requires std::is_base_of_v<IRenderingPipeline, T> {
	std::unique_ptr<T> renderer = std::make_unique<T>(std::forward<Args>(_args)...);
	renderer->Initialize(pShaderCompiler_, pDxManager_);
	renderer3ds_.push_back(std::move(renderer));
}

template<class T, typename... Args>
inline void RenderingPipelineCollection::Generate2DRenderingPipeline(Args&&... _args) requires std::is_base_of_v<IRenderingPipeline, T> {
	std::unique_ptr<T> renderer = std::make_unique<T>(std::forward<Args>(_args)...);
	renderer->Initialize(pShaderCompiler_, pDxManager_);
	renderer2ds_.push_back(std::move(renderer));
}

template<class T, typename... Args>
inline void RenderingPipelineCollection::GeneratePostProcessPipeline(Args&&... _args) requires std::is_base_of_v<IPostProcessPipeline, T> {
	std::unique_ptr<T> postProcess = std::make_unique<T>();
	postProcess->Initialize(pShaderCompiler_, pDxManager_);
	postProcesses_.push_back(std::move(postProcess));
}