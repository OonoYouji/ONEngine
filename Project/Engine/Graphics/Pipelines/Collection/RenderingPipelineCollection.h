#pragma once

/// std
#include <vector>
#include <memory>

/// engine
#include "../Interface/IRenderingPipeline.h"
#include "../Interface/IPostProcessPipeline.h"

template <typename T>
concept IsRenderingPipeline = std::is_base_of_v<IRenderingPipeline, T>;

template <typename T>
concept IsPostProcessPipeline = std::is_base_of_v<IPostProcessPipeline, T>;

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
	template <IsRenderingPipeline T, typename... Args>
	void Generate3DRenderingPipeline(Args&&... _args);

	template <IsRenderingPipeline T, typename... Args>
	void Generate2DRenderingPipeline(Args&&... _args);

	template <IsPostProcessPipeline T, typename... Args>
	void GeneratePostProcessPipeline(Args&&... _args);



	/// @brief すべてのPipelineのPreDrawを実行する
	/// @param _3dCamera 3Dカメラ
	/// @param _2dCamera 2Dカメラ
	void PreDrawEntities(class CameraComponent* _3dCamera, class CameraComponent* _2dCamera);

	/// @brief 現在のECSGroupのすべてのEntityを描画する
	/// @param _3dCamera 3Dカメラ
	/// @param _2dCamera 2Dカメラ
	void DrawEntities(class CameraComponent* _3dCamera, class CameraComponent* _2dCamera);

	/// @brief 選択されたPrefabの描画
	/// @param _3dCamera 3Dカメラ
	/// @param _2dCamera 2Dカメラ
	void DrawSelectedPrefab(class CameraComponent* _3dCamera, class CameraComponent* _2dCamera);


	/// @brief ポストエフェクトの実行
	/// @param _sceneTextureName シーンの名前 (Debug, Game, Prefab etc...)
	void ExecutePostProcess(const std::string& _sceneTextureName);


	/// @brief 引数のカメラが有効なのか確認する
	/// @param _camera チェックしたいカメラ
	/// @return true: 有効, false: 無効
	bool IsEnableCamera(const class CameraComponent* _camera) const;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	class DxManager* pDxManager_;
	class EntityComponentSystem* pEntityComponentSystem_;
	class AssetCollection* pAssetCollection_;
	class ShaderCompiler* pShaderCompiler_;

	std::unique_ptr<IRenderingPipeline> prefabRenderingPipeline_;

	std::vector<std::unique_ptr<IRenderingPipeline>>   renderer3ds_;
	std::vector<std::unique_ptr<IRenderingPipeline>>   renderer2ds_;
	std::vector<std::unique_ptr<IPostProcessPipeline>> postProcesses_;
};



/// ===================================================
/// inline methods
/// ===================================================

template<IsRenderingPipeline T, typename... Args>
inline void RenderingPipelineCollection::Generate3DRenderingPipeline(Args&&... _args) {
	std::unique_ptr<T> renderer = std::make_unique<T>(std::forward<Args>(_args)...);
	renderer->Initialize(pShaderCompiler_, pDxManager_);
	renderer3ds_.push_back(std::move(renderer));
}

template<IsRenderingPipeline T, typename... Args>
inline void RenderingPipelineCollection::Generate2DRenderingPipeline(Args&&... _args) {
	std::unique_ptr<T> renderer = std::make_unique<T>(std::forward<Args>(_args)...);
	renderer->Initialize(pShaderCompiler_, pDxManager_);
	renderer2ds_.push_back(std::move(renderer));
}

template<IsPostProcessPipeline T, typename... Args>
inline void RenderingPipelineCollection::GeneratePostProcessPipeline(Args&&... _args) {
	std::unique_ptr<T> postProcess = std::make_unique<T>();
	postProcess->Initialize(pShaderCompiler_, pDxManager_);
	postProcesses_.push_back(std::move(postProcess));
}