#pragma once

/// std
#include <memory>
#include <vector>
#include <string>

/// engine
#include "../IAsset.h"
#include "Mesh.h"
#include "Skinning.h"

namespace ONEngine {

/// ///////////////////////////////////////////////////
/// Meshの集合体、モデルデータ (アニメーションがある場合も含む)
/// ///////////////////////////////////////////////////
class Model final : public IAsset {
public:

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


	/// ----- animation data ----- ///
	Node rootNode_;
	std::unordered_map<std::string, JointWeightData> jointWeightData_;
	std::unordered_map<std::string, NodeAnimation> nodeAnimationMap_;
	float duration_;


public:
	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// ----- setters ----- ///

	void SetMeshes(std::vector<std::shared_ptr<ModelMesh>>&& _meshes);
	void SetPath(const std::string& _path);
	void SetRootNode(const Node& _node);
	void SetAnimationDuration(float _duration);


	/// ----- getters ----- ///

	/// @brief Modelのソースパスを取得
	const std::string& GetPath() const;

	/// @brief Modelが持つMesh群を取得
	const std::vector<std::shared_ptr<ModelMesh>>& GetMeshes() const;
	std::vector<std::shared_ptr<ModelMesh>>& GetMeshes();

	/// @brief アニメーションのルートノードを取得
	const Node& GetRootNode() const;

	/// @brief アニメーションのJointWeightDataを取得
	const std::unordered_map<std::string, JointWeightData>& GetJointWeightData() const;
	std::unordered_map<std::string, JointWeightData>& GetJointWeightData();

	/// @brief アニメーションのNodeAnimationのマップを取得
	const std::unordered_map<std::string, NodeAnimation>& GetNodeAnimationMap() const;
	std::unordered_map<std::string, NodeAnimation>& GetNodeAnimationMap();

	/// @brief アニメーションの再生時間を取得
	float GetAnimationDuration() const;


};

} /// ONEngine
