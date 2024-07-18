#pragma once


class PipelineState;



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
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();


private:
	ModelManager(const ModelManager&) = delete;
	ModelManager(ModelManager&&) = delete;
	ModelManager& operator= (const ModelManager&) = delete;
	ModelManager& operator= (ModelManager&&) = delete;
};