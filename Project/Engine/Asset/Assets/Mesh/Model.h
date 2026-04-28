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

	/// @brief メッシュごとのスキンデータ
	struct SkinData {
		std::unordered_map<std::string, JointWeightData> jointWeightData;
	};

	struct ModelAnimation {
		std::string name;
		float duration;
		std::unordered_map<std::string, NodeAnimation> nodeAnimationMap;
	};

	/// ===================================================
	/// public : methods
	/// ===================================================

	Model();
	~Model() override;

	/// @brief mesh の新規追加
	/// @param _mesh meshのunique_ptr
	void AddMesh(std::shared_ptr<ModelMesh>&& _mesh, const SkinData& _skinData = {});

	ModelMesh* CreateMesh();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::vector<std::shared_ptr<ModelMesh>> meshes_;
	std::vector<SkinData>              skinDatas_;
	std::string                        path_;


	/// ----- animation data ----- ///
	Node rootNode_;
	std::vector<ModelAnimation> animations_;


public:
	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// ----- setters ----- ///

	void SetPath(const std::string& _path);
	void SetRootNode(const Node& _node);


	/// ----- getters ----- ///

	/// @brief Modelのソースパスを取得
	const std::string& GetPath() const;

	/// @brief Modelが持つMesh群を取得
	const std::vector<std::shared_ptr<ModelMesh>>& GetMeshes() const;
	std::vector<std::shared_ptr<ModelMesh>>& GetMeshes();

	/// @brief メッシュごとのスキンデータを取得
	const std::vector<SkinData>& GetSkinDatas() const { return skinDatas_; }

	/// @brief アニメーションのルートノードを取得
	const Node& GetRootNode() const;

	/// @brief アニメーション群を取得
	const std::vector<ModelAnimation>& GetAnimations() const { return animations_; }
	std::vector<ModelAnimation>& GetAnimations() { return animations_; }


};

} /// ONEngine::Asset
