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


/// ///////////////////////////////////////////////////
/// Meshの集合体、モデルデータ (アニメーションがある場合も含む)
/// ///////////////////////////////////////////////////
class Model final : public IAsset {
public:

	/// @brief Model用のメタデータ
	struct MetaData {
		float scale;
	};


	struct Vertex {
		Vector4 position;
		Vector2 uv;
		Vector3 normal;
	};

	using ModelMesh = Mesh<Vertex>;

	/// ===================================================
	/// public : methods
	/// ===================================================

	Model();
	~Model() override;

	/// @brief mesh の新規追加
	/// @param _mesh meshのunique_ptr
	void AddMesh(std::shared_ptr<ModelMesh>&& _mesh);

	ModelMesh* CreateMesh();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::vector<std::shared_ptr<ModelMesh>> meshes_;
	std::string                        path_;


	/// ----- skeleton & skinning data ----- ///
	Node rootNode_;
	std::vector<std::unordered_map<uint32_t, JointWeightData>> meshJointWeightData_;
	
	/// ----- animation clips ----- ///
	std::unordered_map<uint32_t, AnimationClip> animationClips_;


public:
	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// ----- setters ----- ///

	void SetMeshes(std::vector<std::shared_ptr<ModelMesh>>&& _meshes);
	void SetPath(const std::string& _path);
	void SetRootNode(const Node& _node);


	/// ----- getters ----- ///

	/// @brief Modelのソースパスを取得
	const std::string& GetPath() const;

	/// @brief Modelが持つMesh群を取得
	const std::vector<std::shared_ptr<ModelMesh>>& GetMeshes() const;
	std::vector<std::shared_ptr<ModelMesh>>& GetMeshes();

	/// @brief アニメーションのルートノードを取得
	const Node& GetRootNode() const;

	/// @brief アニメーションのJointWeightDataを取得 (メッシュごとのリスト)
	const std::vector<std::unordered_map<uint32_t, JointWeightData>>& GetMeshJointWeightData() const;
	std::vector<std::unordered_map<uint32_t, JointWeightData>>& GetMeshJointWeightData();

	/// @brief アニメーションクリップのマップを取得
	const std::unordered_map<uint32_t, AnimationClip>& GetAnimationClips() const;
	std::unordered_map<uint32_t, AnimationClip>& GetAnimationClips();


};

} /// ONEngine::Asset
