#include "Model.h"

using namespace ONEngine;

Model::Model() = default;
Model::~Model() = default;

void Model::AddMesh(std::shared_ptr<Mesh>&& _mesh) {
	meshes_.push_back(std::move(_mesh));
}

Mesh* Model::CreateMesh() {
	/// ----- 新規Meshを追加し、返す ----- ///
	meshes_.emplace_back(std::make_shared<Mesh>());
	return meshes_.back().get();
}

void Model::SetMeshes(std::vector<std::shared_ptr<Mesh>>&& _meshes) {
	/// ----- 新しいMeshと今のMeshを入れ替える ----- ///
	if (_meshes.size() > meshes_.size()) {
		meshes_.resize(_meshes.size());
	}

	for (size_t i = 0; i < _meshes.size(); ++i) {
		meshes_[i] = std::move(_meshes[i]);
	}
}

const std::vector<std::shared_ptr<Mesh>>& Model::GetMeshes() const {
	return meshes_;
}

std::vector<std::shared_ptr<Mesh>>& Model::GetMeshes() {
	return meshes_;
}

void Model::SetPath(const std::string& _path) {
	path_ = _path;
}

void Model::SetRootNode(const Node& _node) {
	rootNode_ = _node;
}

void Model::SetAnimationDuration(float _duration) {
	duration_ = _duration;
}

const std::string& Model::GetPath() const {
	return path_;
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
