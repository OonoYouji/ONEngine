#pragma once

/// std
#include <memory>
#include <unordered_map>
#include <vector>
#include <list>

/// external
#include <assimp/scene.h>

#include "GraphicManager/PipelineState/PipelineState.h"
#include "Model.h"


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
	static ModelManager* GetInstance() {
		static ModelManager instance;
		return &instance;
	}

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
	static Model* Load(const std::string& filePath);

	Node ReadNode(aiNode* node);

	/// <summary>
	/// モデルのゲッタ
	/// </summary>
	/// <param name="filePath"></param>
	/// <returns></returns>
	static Model* GetModel(const std::string& filePath);


	/// <summary>
	/// 平面の生成
	/// </summary>
	/// <returns></returns>
	static Model* CreatePlane();

	/// <summary>
	/// 立方体の生成
	/// </summary>
	/// <returns></returns>
	static Model* CreateCube();

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
	void AddModel(const std::string& filePath, Model* model);

	/// <summary>
	/// command list に pipeline state をセット
	/// </summary>
	void SetPipelineState(FillMode fillMode);

	/// <summary>
	/// アクティブなモデルの追加
	/// </summary>
	void AddActiveModel(Model* model, Transform* transform, Mat4* matLocal, Material* material, FillMode fillMode);

	void SetDirectionalLight(class DirectionalLight* directionalLight);

	/// <summary>
	/// ディレクトリパスのゲッタ
	/// </summary>
	/// <returns></returns>
	const std::string& GetDirectoryPath() const { return kDirectoryPath_; }

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	const std::string kDirectoryPath_ = "./Resources/Models/";


	std::vector<std::unique_ptr<PipelineState>> pipelines_;
	PipelineState::Shader shader_{};

	std::unordered_map<std::string, std::unique_ptr<Model>> models_;

	struct Element final {
		Model*     model     = nullptr;
		Mat4*      matLocal  = nullptr;
		Material*  material  = nullptr;
		Transform* transform = nullptr;
		FillMode   fillMode;
	};

	std::list<Element> activeModels_;

	class DirectionalLight* pDirectionalLight_ = nullptr;

private:
	ModelManager(const ModelManager&) = delete;
	ModelManager(ModelManager&&) = delete;
	ModelManager& operator= (const ModelManager&) = delete;
	ModelManager& operator= (ModelManager&&) = delete;
};