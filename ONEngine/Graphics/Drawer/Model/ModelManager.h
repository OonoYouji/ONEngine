#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include <list>

#include <PipelineState.h>
#include <Model.h>


/// ===================================================
/// モデルの管理クラス
/// ===================================================
class ModelManager final {
	ModelManager() = default;
	~ModelManager() = default;
public:

	/// <summary>
	/// インスタンス確保
	/// </summary>
	/// <returns></returns>
	static ModelManager* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();



	/// <summary>
	/// モデル読み込み
	/// </summary>
	Model* Load(const std::string& filePath);

	/// <summary>
	/// 平面の生成
	/// </summary>
	/// <returns></returns>
	Model* CreatePlane();

	/// <summary>
	/// 立方体の生成
	/// </summary>
	/// <returns></returns>
	Model* CreateCube();

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();


	Model* GetModel(const std::string& filePath);

	/// <summary>
	/// モデルの追加
	/// </summary>
	/// <param name="model"></param>
	void AddModel(const std::string& filePath,Model* model);

	/// <summary>
	/// command list に pipeline state をセット
	/// </summary>
	void SetPipelineState(FillMode fillMode);

	/// <summary>
	/// アクティブなモデルの追加
	/// </summary>
	void AddActiveModel(Model* model);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	const std::string kDirectoryPath_ = "./Resources/Models/";


	std::vector<std::unique_ptr<PipelineState>> pipelines_;
	PipelineState::Shader shader_{};

	std::unordered_map<std::string, std::unique_ptr<Model>> models_;
	std::list<Model*> activeModels_;

	
	ComPtr<ID3D12Resource> viewProjectionBuffer_ = nullptr;
	ViewProjectionData* viewProjectionData_ = nullptr;

private:
	ModelManager(const ModelManager&) = delete;
	ModelManager(ModelManager&&) = delete;
	ModelManager& operator= (const ModelManager&) = delete;
	ModelManager& operator= (ModelManager&&) = delete;
};