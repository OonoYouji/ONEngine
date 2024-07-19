#pragma once

#include <memory>
#include <list>
#include <vector>

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
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	/// <summary>
	/// モデルの追加
	/// </summary>
	/// <param name="model"></param>
	void AddModel(Model* model);

	/// <summary>
	/// command list に pipeline state をセット
	/// </summary>
	void SetPipelineState(FillMode fillMode);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::vector<std::unique_ptr<PipelineState>> pipelines_;
	PipelineState::Shader shader_{};

	std::list<std::unique_ptr<Model>> models_;

	
	ComPtr<ID3D12Resource> viewProjectionBuffer_ = nullptr;
	ViewProjectionData* viewProjectionData_ = nullptr;

private:
	ModelManager(const ModelManager&) = delete;
	ModelManager(ModelManager&&) = delete;
	ModelManager& operator= (const ModelManager&) = delete;
	ModelManager& operator= (ModelManager&&) = delete;
};