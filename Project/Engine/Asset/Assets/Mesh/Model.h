#pragma once

/// std
#include <memory>
#include <vector>
#include <string>

/// engine
#include "../IAsset.h"
#include "Mesh.h"
#include "Skinning.h"

namespace ONEngine::Asset {


/**
 * @class Model
 * @brief 描画用のメッシュ（複数）やスケルトン、アニメーションクリップなどを保持するモデルアセットクラス
 */
class Model final : public IAsset {
public:

	/**
	 * @struct MetaData
	 * @brief モデルアセット固有のメタデータ
	 */
	struct MetaData {
		float scale; ///< モデルの読み込みスケール倍率
	};

	/**
	 * @struct Vertex
	 * @brief モデルのメッシュで使用する標準的な頂点レイアウト構造体
	 */
	struct Vertex {
		Vector4 position; ///< 頂点座標
		Vector2 uv;       ///< テクスチャ座標
		Vector3 normal;   ///< 法線ベクトル
	};

	using ModelMesh = Mesh<Vertex>;

	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	Model();

	/**
	 * @brief デストラクタ
	 */
	~Model() override;

	/**
	 * @brief モデルにメッシュ（ModelMesh）を追加します。
	 * @param _mesh 追加するメッシュの shared_ptr 右辺参照
	 */
	void AddMesh(std::shared_ptr<ModelMesh>&& _mesh);

	/**
	 * @brief 空のメッシュ（ModelMesh）を新規生成してモデルに追加します。
	 * @return 追加されたメッシュオブジェクトへの生ポインタ
	 */
	ModelMesh* CreateMesh();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::vector<std::shared_ptr<ModelMesh>> meshes_; ///< モデルを構成するメッシュリスト
	std::string                        path_;   ///< アセットソースのファイルパス


	/// ----- skeleton & skinning data ----- ///
	Node rootNode_;                                                           ///< スケルトン木構造のルートノード
	std::vector<std::unordered_map<uint32_t, JointWeightData>> meshJointWeightData_; ///< 各メッシュの頂点に対するジョイントウェイトマップリスト
	
	/// ----- animation clips ----- ///
	std::unordered_map<uint32_t, AnimationClip> animationClips_;              ///< ジョイント別（ID別）のインポートされたアニメーションクリップマップ


public:
	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// ----- setters ----- ----- ///

	/**
	 * @brief メッシュリストをセットします。
	 * @param _meshes メッシュの shared_ptr 配列
	 */
	void SetMeshes(std::vector<std::shared_ptr<ModelMesh>>&& _meshes);

	/**
	 * @brief ソースアセットのファイルパスを設定します。
	 * @param _path アセットのファイルパス
	 */
	void SetPath(const std::string& _path);

	/**
	 * @brief スケルトンのルートノードをセットします。
	 * @param _node ルートノード
	 */
	void SetRootNode(const Node& _node);


	/// ----- getters ----- ///

	/**
	 * @brief モデルアセットのファイルパスを取得します。
	 * @return パス文字列の定数参照
	 */
	const std::string& GetPath() const;

	/**
	 * @brief モデルが持つメッシュリストを取得（読み取り専用）します。
	 * @return メッシュリストの参照
	 */
	const std::vector<std::shared_ptr<ModelMesh>>& GetMeshes() const;

	/**
	 * @brief モデルが持つメッシュリストを取得します。
	 * @return メッシュリストの参照
	 */
	std::vector<std::shared_ptr<ModelMesh>>& GetMeshes();

	/**
	 * @brief スケルトンのルートノードを取得します。
	 * @return ルートノードの定数参照
	 */
	const Node& GetRootNode() const;

	/**
	 * @brief ジョイントウェイトマップのリストを取得（読み取り専用）します。
	 * @return ジョイントウェイトのマップ配列
	 */
	const std::vector<std::unordered_map<uint32_t, JointWeightData>>& GetMeshJointWeightData() const;

	/**
	 * @brief ジョイントウェイトマップのリストを取得します。
	 * @return ジョイントウェイトのマップ配列
	 */
	std::vector<std::unordered_map<uint32_t, JointWeightData>>& GetMeshJointWeightData();

	/**
	 * @brief インポートされたアニメーションクリップのマップを取得（読み取り専用）します。
	 * @return アニメーションクリップマップ
	 */
	const std::unordered_map<uint32_t, AnimationClip>& GetAnimationClips() const;

	/**
	 * @brief インポートされたアニメーションクリップのマップを取得します。
	 * @return アニメーションクリップマップ
	 */
	std::unordered_map<uint32_t, AnimationClip>& GetAnimationClips();


};

} /// ONEngine::Asset
