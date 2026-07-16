#pragma once

/// engine
#include "../IAssetLoader.h"
#include "../../Meta/MetaFile.h"
#include "Model.h"

struct aiNode;
struct aiScene;

namespace ONEngine::Asset {

/**
 * @class AssetLoader<Model>
 * @brief Modelアセット（FBX, OBJ等）専用のアセットローダーの特殊化クラス。Assimpライブラリを使用します。
 */
template<>
class AssetLoader<Model> : public IAssetLoader {
public:
	/// ====================================================
	/// public : methods
	/// ====================================================

	/**
	 * @brief コンストラクタ。DirectXのマネージャーを受け取り初期化します。
	 * @param _dxm DirectX12デバイスなどを管理するマネージャのポインタ
	 */
	AssetLoader(DxManager* _dxm);

	/**
	 * @brief デストラクタ
	 */
	~AssetLoader() override = default;

	/**
	 * @brief ディスクからモデルアセットファイル（FBX, OBJ等）を読み込みます。
	 * @param _filepath 読み込み対象のファイルパス
	 * @param meta モデルアセットのメタデータ
	 * @return ロードされたModelアセット（失敗時はstd::nullopt）
	 */
	[[nodiscard]]
	std::optional<Model> Load(const std::string& _filepath, typename Meta<Model::MetaData> meta);

	/**
	 * @brief 既存のモデルに対して再ロード（リロード）を実行します。
	 * @param _filepath 再ロード対象のファイルパス
	 * @param _src 再ロード元のModelオブジェクトへのポインタ
	 * @param meta モデルアセットのメタデータ
	 * @return 再ロードされたModelアセット（失敗時はstd::nullopt）
	 */
	[[nodiscard]]
	std::optional<Model> Reload(const std::string& _filepath, Model* _src = nullptr, typename Meta<Model::MetaData> meta = {});

	/**
	 * @brief モデルに対応するメタデータを取得します。
	 * @param _filepath 対象アセットファイルのパス
	 * @return 解析・構築されたメタデータオブジェクト
	 */
	Meta<typename Model::MetaData> GetMetaData(const std::string& _filepath);

private:

	/**
	 * @brief AssimpのaiNodeからスケルトン用ノード（Node）を再帰的に読み込みます。
	 * @param _node 走査対象のaiNodeポインタ
	 * @return 構築されたNode構造体
	 */
	Node ReadNode(aiNode* _node);

	/**
	 * @brief AssimpのaiSceneに含まれるアニメーションデータを解析し、Modelにインポートします。
	 * @param _model インポート先のModelポインタ
	 * @param _scene AssimpのaiSceneポインタ
	 */
	void LoadAnimation(Model* _model, const aiScene* _scene);

	/**
	 * @brief ロードしたシーンデータがエンジン側で処理可能な構成であるかを検証します。
	 * @param _aiScene 解析されたAssimpのaiSceneポインタ
	 * @return 処理可能な場合はtrue、不適合なデータ（メッシュなしなど）の場合はfalse
	 */
	bool ValidateModel(const aiScene* _aiScene);

private:
	/// ====================================================
	/// private : objects
	/// ====================================================

	DxManager* pDxManager_;      ///< グラフィックスデバイスマネージャのポインタ
	uint32_t assimpLoadFlags_;  ///< Assimpインポート用のプロセスフラグ
};

} /// namespace ONEngine::Asset