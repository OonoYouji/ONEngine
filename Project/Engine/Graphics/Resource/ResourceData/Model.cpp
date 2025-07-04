#include "Model.h"

Model::Model() {}
Model::~Model() {}

void Model::SetMeshes(std::vector<std::unique_ptr<Mesh>>&& _meshes) {
	if (_meshes.size() > meshes_.size()) {
		meshes_.resize(_meshes.size());
	}

	for (size_t i = 0; i < _meshes.size(); ++i) {
		meshes_[i] = std::move(_meshes[i]);
	}
}

const std::vector<std::unique_ptr<Mesh>>& Model::GetMeshes() const {
	return meshes_;
}

std::vector<std::unique_ptr<Mesh>>& Model::GetMeshes() {
	return meshes_;
}

void Model::SetRootNode(const Node& _node) {
	rootNode_ = _node;
}

const Node& Model::GetRootNode() const {
	return rootNode_;
}

const std::unordered_map<std::string, JointWeightData>& Model::GetSkinClusterData() const {
	return skinClusters_;
}

std::unordered_map<std::string, JointWeightData>& Model::GetSkinClusterData() {
	return skinClusters_;
}
