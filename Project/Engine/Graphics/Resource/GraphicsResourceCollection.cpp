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
			meshes_.erase(path);

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

	meshes_[_filePath] = std::move(_mesh);
}
