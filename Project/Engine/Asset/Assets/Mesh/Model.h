#pragma once

/// std
#include <memory>
#include <vector>
#include <string>

/// engine
#include "../Asset.h"
#include "Mesh.h"
#include "Skinning.h"

/// ===================================================
/// mesh描画クラス
/// ===================================================
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

	/// @brief mesh配列のセッタ
	/// @param _meshes shared_ptrのmesh配列
	void SetMeshes(std::vector<std::shared_ptr<Mesh>>&& _meshes);

	/// @brief パスのセッタ
	/// @param _path ファイルパス
	void SetPath(const std::string& _path);

	void SetRootNode(const Node& _node);

	void SetAnimationDuration(float _duration);


	/// @brief meshの配列のゲッタ
	/// @return meshの配列
	const std::vector<std::shared_ptr<Mesh>>& GetMeshes() const;
	std::vector<std::shared_ptr<Mesh>>& GetMeshes();

	/// @brief 読み込みに使用したファイルパスのゲッタ
	/// @return pathの文字列
	const std::string& GetPath() const;


	const Node& GetRootNode() const;

	const std::unordered_map<std::string, JointWeightData>& GetJointWeightData() const;
	std::unordered_map<std::string, JointWeightData>& GetJointWeightData();

	const std::unordered_map<std::string, NodeAnimation>& GetNodeAnimationMap() const;
	std::unordered_map<std::string, NodeAnimation>& GetNodeAnimationMap();

	float GetAnimationDuration() const;


};
