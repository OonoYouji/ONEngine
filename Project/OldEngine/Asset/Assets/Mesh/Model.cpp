#include "Model.h"


namespace ONEngine::Asset {

Model::Model() = default;
Model::~Model() = default;

void Model::AddMesh(std::shared_ptr<ModelMesh>&& _mesh, const SkinData& _skinData) {
	meshes_.push_back(std::move(_mesh));
	skinDatas_.push_back(_skinData);
}

Model::ModelMesh* Model::CreateMesh() {
	/// ----- 新規Meshを追加し、返す ----- ///
	meshes_.emplace_back(std::make_shared<ModelMesh>());
	skinDatas_.emplace_back();
	return meshes_.back().get();
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

} // namespace ONEngine::Asset
