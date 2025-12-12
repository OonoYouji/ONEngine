#pragma once

/// engine
#include "../IAssetLoader.h"
#include "Model.h"

struct aiNode;
struct aiScene;

namespace ONEngine {

template<>
class AssetLoaderT<Model> : public IAssetLoader {
public:
	/// ====================================================
	/// public : methods
	/// ====================================================

	AssetLoaderT(DxManager* _dxm);
	~AssetLoaderT() override = default;

	/// @brief モデルファイルの読み込みを行う
	/// @param _filepath 対象のファイルパス
	/// @return 読み込んだモデル
	[[nodiscard]]
	Model Load(const std::string& _filepath);

	/// @brief モデルファイルの再読み込みを行う
	/// @param _filepath 対象のファイルパス
	/// @param _src 元のモデル
	/// @return 再読み込みしたモデル
	[[nodiscard]]
	Model Reload(const std::string& _filepath, Model* _src);


	/// @brief アニメーションのNodeを読み込む
	/// @param _node 読み込み対象のaiNodeポインタ
	/// @return 読み込まれたNode構造体
	Node ReadNode(aiNode* _node);

	/// @brief アニメーションの読み込み
	/// @param _model 読み込み対象のModelポインタ
	/// @param _filepath 読み込み対象のファイルパス
	void LoadAnimation(Model* _model, const std::string& _filepath);

private:

	/// @brief 読み込めるモデルであるのかチェックする
	/// @param _aiScene チェック対象のモデル
	/// @return true: 読み込める / false: 読み込めない
	bool ValidateModel(const aiScene* _aiScene);

private:
	/// ====================================================
	/// private : objects
	/// ====================================================

	DxManager* pDxManager_;
	uint32_t assimpLoadFlags_;
};

} /// namespace ONEngine