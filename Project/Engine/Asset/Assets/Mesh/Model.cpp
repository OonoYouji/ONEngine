#include "Model.h"


namespace ONEngine::Asset {

/**
 * @brief コンストラクタ
 */
Model::Model() = default;

/**
 * @brief デストラクタ
 */
Model::~Model() = default;

/**
 * @brief モデルにメッシュ（ModelMesh）を追加します。
 * @param _mesh 追加するメッシュの shared_ptr 右辺参照
 */
void Model::AddMesh(std::shared_ptr<ModelMesh>&& _mesh) {
	meshes_.push_back(std::move(_mesh));
}

/**
 * @brief 空のメッシュ（ModelMesh）を新規生成してモデルに追加します。
 * @return 追加されたメッシュオブジェクトへの生ポインタ
 */
Model::ModelMesh* Model::CreateMesh() {
	/// ----- 新規Meshを追加し、返す ----- ///
	meshes_.emplace_back(std::make_shared<ModelMesh>());
	return meshes_.back().get();
}

/**
 * @brief メッシュリストをセット（ムーブ）します。
 * @param _meshes メッシュの shared_ptr 配列
 */
void Model::SetMeshes(std::vector<std::shared_ptr<ModelMesh>>&& _meshes) {
	/// ----- 新しいMeshと今のMeshを入れ替える ----- ///
	if(_meshes.size() > meshes_.size()) {
		meshes_.resize(_meshes.size());
	}

	for(size_t i = 0; i < _meshes.size(); ++i) {
		meshes_[i] = std::move(_meshes[i]);
	}
}

/**
 * @brief モデルが持つメッシュリストを取得（読み取り専用）します。
 * @return メッシュリストの参照
 */
const std::vector<std::shared_ptr<Model::ModelMesh>>& Model::GetMeshes() const {
	return meshes_;
}

/**
 * @brief モデルが持つメッシュリストを取得します。
 * @return メッシュリストの参照
 */
std::vector<std::shared_ptr<Model::ModelMesh>>& Model::GetMeshes() {
	return meshes_;
}

/**
 * @brief ソースアセットのファイルパスを設定します。
 * @param _path アセットのファイルパス
 */
void Model::SetPath(const std::string& _path) {
	path_ = _path;
}

/**
 * @brief スケルトンのルートノードをセットします。
 * @param _node ルートノード
 */
void Model::SetRootNode(const Node& _node) {
	rootNode_ = _node;
}

/**
 * @brief モデルアセットのファイルパスを取得します。
 * @return パス文字列の定数参照
 */
const std::string& Model::GetPath() const {
	return path_;
}

/**
 * @brief スケルトンのルートノードを取得します。
 * @return ルートノードの定数参照
 */
const Node& Model::GetRootNode() const {
	return rootNode_;
}

/**
 * @brief ジョイントウェイトマップのリストを取得（読み取り専用）します。
 * @return ジョイントウェイトのマップ配列
 */
const std::vector<std::unordered_map<uint32_t, JointWeightData>>& Model::GetMeshJointWeightData() const {
	return meshJointWeightData_;
}

/**
 * @brief ジョイントウェイトマップのリストを取得します。
 * @return ジョイントウェイトのマップ配列
 */
std::vector<std::unordered_map<uint32_t, JointWeightData>>& Model::GetMeshJointWeightData() {
	return meshJointWeightData_;
}

/**
 * @brief インポートされたアニメーションクリップのマップを取得（読み取り専用）します。
 * @return アニメーションクリップマップ
 */
const std::unordered_map<uint32_t, AnimationClip>& Model::GetAnimationClips() const {
	return animationClips_;
}

/**
 * @brief インポートされたアニメーションクリップのマップを取得します。
 * @return アニメーションクリップマップ
 */
std::unordered_map<uint32_t, AnimationClip>& Model::GetAnimationClips() {
	return animationClips_;
}

} // namespace ONEngine::Asset