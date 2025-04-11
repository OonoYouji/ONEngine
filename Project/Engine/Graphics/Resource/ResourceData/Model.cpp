#include "Model.h"

Model::Model() {}
Model::~Model() {}

void Model::SetMeshes(std::vector<std::unique_ptr<Mesh>>&& _meshes) {
	if (_meshes.size() > meshes_.size()) {
		meshes_.resize(_meshes.size());
	}

	for (size_t i = 0; i < _meshes.size(); ++i) {
		//meshes_[i].swap(std::move(_meshes[i]));
		//meshes_[i] = std::move(_meshes[i]);
	}
}
