#include "GraphicsResourceCollection.h"

GraphicsResourceCollection::GraphicsResourceCollection() {}
GraphicsResourceCollection::~GraphicsResourceCollection() {}

void GraphicsResourceCollection::Initialize(DxManager* _dxManager) {
	resourceLoader_ = std::make_unique<GraphicsResourceLoader>(_dxManager, this);
	resourceLoader_->Initialize();
}

void GraphicsResourceCollection::LoadResources(const std::vector<std::string>& _filePaths) {

	for (auto& path : _filePaths) {
		Type type = Type::none;

		if (path.find(".png") != std::string::npos || path.find(".jpg") != std::string::npos) {
			type = Type::texture;
		} else if (path.find(".obj") != std::string::npos) {
			type = Type::model;
		}

		Load(path, type);
	}

}

void GraphicsResourceCollection::UnloadResources(const std::vector<std::string>& _filePaths) {

	for (auto& path : _filePaths) {
		Type type = Type::none;

		if (path.find(".png") != std::string::npos || path.find(".jpg") != std::string::npos) {
			type = Type::texture;
		} else if (path.find(".obj") != std::string::npos) {
			type = Type::model;
		}


		switch (type) {
		case GraphicsResourceCollection::Type::none:
			continue; ///< noneの場合は何もしない
		case GraphicsResourceCollection::Type::texture:

			break;
		case GraphicsResourceCollection::Type::model:

			/// meshの解放
			auto itr = meshIndices_.find(path);
			if (itr != meshIndices_.end()) {

				/// 解放するmeshのindexをfreeIndices_に追加
				freeIndices_.push_back(itr->second);

				/// meshの解放
				meshes_[itr->second].reset();
				meshIndices_.erase(itr);
			}

			break;
		}

	}

}

void GraphicsResourceCollection::Load(const std::string& _filePath, Type _type) {

	///< noneの場合は読み込まない
	if (_type == Type::none) {
		return;
	}

	switch (_type) {
	case GraphicsResourceCollection::Type::texture:
		resourceLoader_->LoadTexture(_filePath);
		break;
	case GraphicsResourceCollection::Type::model:
		resourceLoader_->LoadModelObj(_filePath);
		break;
	}

}

void GraphicsResourceCollection::AddMesh(const std::string& _filePath, std::unique_ptr<Mesh>& _mesh) {

	if (!freeIndices_.empty()) { ///< 空いているindexがある場合はそこに追加
		size_t index = freeIndices_.back();
		freeIndices_.pop_back();

		meshIndices_[_filePath] = index;
		meshes_[index] = std::move(_mesh);
		return;
	}


	meshIndices_[_filePath] = meshes_.size();
	meshes_.push_back(std::move(_mesh));
}
