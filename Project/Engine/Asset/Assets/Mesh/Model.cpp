#include "Model.h"


namespace ONEngine::Asset {

Model::Model() = default;
Model::~Model() = default;

void Model::AddMesh(std::shared_ptr<ModelMesh>&& _mesh) {
	meshes_.push_back(std::move(_mesh));
}

Model::ModelMesh* Model::CreateMesh() {
	/// ----- 新規Meshを追加し、返す ----- ///
	meshes_.emplace_back(std::make_shared<ModelMesh>());
	return meshes_.back().get();
}

void Model::SetMeshes(std::vector<std::shared_ptr<ModelMesh>>&& _meshes) {
	/// ----- 新しいMeshと今のMeshを入れ替える ----- ///
	if(_meshes.size() > meshes_.size()) {
		meshes_.resize(_meshes.size());
	}

	for(size_t i = 0; i < _meshes.size(); ++i) {
		meshes_[i] = std::move(_meshes[i]);
	}
}

const std::vector<std::shared_ptr<Model::ModelMesh>>& Model::GetMeshes() const {
	return meshes_;
}

std::vector<std::shared_ptr<Model::ModelMesh>>& Model::GetMeshes() {
	return meshes_;
}

void Model::SetPath(const std::string& _path) {
	path_ = _path;
}

void Model::SetRootNode(const Node& _node) {
	rootNode_ = _node;
}

const std::string& Model::GetPath() const {
	return path_;
}

const Node& Model::GetRootNode() const {
	return rootNode_;
}

const std::vector<std::unordered_map<uint32_t, JointWeightData>>& Model::GetMeshJointWeightData() const {
	return meshJointWeightData_;
}

std::vector<std::unordered_map<uint32_t, JointWeightData>>& Model::GetMeshJointWeightData() {
	return meshJointWeightData_;
}

const std::unordered_map<uint32_t, AnimationClip>& Model::GetAnimationClips() const {
	return animationClips_;
}

std::unordered_map<uint32_t, AnimationClip>& Model::GetAnimationClips() {
	return animationClips_;
}

} // namespace ONEngine::Asset