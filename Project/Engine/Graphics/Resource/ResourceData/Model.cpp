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

void Model::SetAnimationDuration(float _duration) {
	duration_ = _duration;
}

const Node& Model::GetRootNode() const {
	return rootNode_;
}

const std::unordered_map<std::string, JointWeightData>& Model::GetJointWeightData() const {
	return jointWeightData_;
}

std::unordered_map<std::string, JointWeightData>& Model::GetJointWeightData() {
	return jointWeightData_;
}

const std::unordered_map<std::string, NodeAnimation>& Model::GetNodeAnimationMap() const {
	return nodeAnimationMap_;
}

std::unordered_map<std::string, NodeAnimation>& Model::GetNodeAnimationMap() {
	return nodeAnimationMap_;
}

float Model::GetAnimationDuration() const {
	return duration_;
}
