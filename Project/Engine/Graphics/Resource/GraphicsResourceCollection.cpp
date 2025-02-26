#include "GraphicsResourceCollection.h"

GraphicsResourceCollection::GraphicsResourceCollection() {}
GraphicsResourceCollection::~GraphicsResourceCollection() {}

void GraphicsResourceCollection::Initialize(DxManager* _dxManager) {
	resourceLoader_ = std::make_unique<GraphicsResourceLoader>(_dxManager, this);
	resourceLoader_->Initialize();

	textures_.resize(32);
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
			models_.erase(path);

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
		
		/// 読み込み済みかチェックし、読み込んでいない場合のみ読み込む
		if (GetTexture(_filePath) == nullptr) {
			resourceLoader_->LoadTexture(_filePath);
		}

		break;
	case GraphicsResourceCollection::Type::model:
		resourceLoader_->LoadModelObj(_filePath);
		break;
	}

}

void GraphicsResourceCollection::AddModel(const std::string& _filePath, std::unique_ptr<Model> _model) {
	models_[_filePath] = std::move(_model);
}

void GraphicsResourceCollection::AddTexture(const std::string& _filePath, std::unique_ptr<Texture> _texture) {
	textureIndices_[_filePath] = textureIndices_.size();
	textures_[textureIndices_[_filePath]] = std::move(_texture);
}

const Model* GraphicsResourceCollection::GetModel(const std::string& _filePath) const {
	auto itr = models_.find(_filePath);
	if (itr != models_.end()) {
		return itr->second.get();
	}

	return nullptr;
}

const Texture* GraphicsResourceCollection::GetTexture(const std::string& _filePath) const {
	auto itr = textureIndices_.find(_filePath);
	if (itr != textureIndices_.end()) {
		return textures_[itr->second].get();
	}

	return nullptr;
}

size_t GraphicsResourceCollection::GetTextureIndex(const std::string& _filePath) const {
	return textureIndices_.at(_filePath);
}
