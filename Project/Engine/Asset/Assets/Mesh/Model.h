#pragma once

/// std
#include <memory>
#include <vector>
#include <string>

/// engine
#include "../Asset.h"
#include "Mesh.h"
#include "Skinning.h"

/// ///////////////////////////////////////////////////
/// Meshの集合体、モデルデータ (アニメーションがある場合も含む)
/// ///////////////////////////////////////////////////
class Model final : public IAsset {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	Model();
	~Model() override;

	/// @brief mesh の新規追加
	/// @param _mesh meshのunique_ptr
	void AddMesh(std::shared_ptr<Mesh>&& _mesh);

	Mesh* CreateMesh();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::vector<std::shared_ptr<Mesh>> meshes_;
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

	void SetMeshes(std::vector<std::shared_ptr<Mesh>>&& _meshes);
	void SetPath(const std::string& _path);
	void SetRootNode(const Node& _node);
	void SetAnimationDuration(float _duration);


	/// ----- getters ----- ///

	/// @brief Modelのソースパスを取得
	const std::string& GetPath() const;

	/// @brief Modelが持つMesh群を取得
	const std::vector<std::shared_ptr<Mesh>>& GetMeshes() const;
	std::vector<std::shared_ptr<Mesh>>& GetMeshes();

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
