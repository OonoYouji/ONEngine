#include "GraphicsResourceCollection.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Creator/PrimitiveMeshCreator.h"

GraphicsResourceCollection::GraphicsResourceCollection() {}
GraphicsResourceCollection::~GraphicsResourceCollection() {}

#include <filesystem>

void GraphicsResourceCollection::Initialize(DxManager* _dxManager) {
	resourceLoader_ = std::make_unique<GraphicsResourceLoader>(_dxManager, this);
	resourceLoader_->Initialize();

	textures_.resize(32);

	/// Packages内のファイルがすべて読み込む
	std::vector<std::string> texturePaths;
	for (const auto& entry : std::filesystem::directory_iterator("./Packages/Textures")) {
		if (entry.is_regular_file()) {
			std::string path = entry.path().string();
			
			/// パスの中にある "\\" を "/" に置き換える
			std::replace(path.begin(), path.end(), '\\', '/');
			
			if (path.find(".png") != std::string::npos 
				|| path.find(".jpg") != std::string::npos
				|| path.find(".dds") != std::string::npos) {
				texturePaths.push_back(path);
			}
		}
	}

	LoadResources(texturePaths);

	/// primitive meshを作成
	PrimitiveMeshCreator primitiveMeshCreator(this, _dxManager->GetDxDevice());
}

void GraphicsResourceCollection::LoadResources(const std::vector<std::string>& _filePaths) {

	for (auto& path : _filePaths) {
		Type type = Type::none;

		if (path.find(".png") != std::string::npos
			|| path.find(".jpg") != std::string::npos
			|| path.find(".dds") != std::string::npos) {
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
	if (_filePath == "") {
		return 0;
	}

	return textureIndices_.at(_filePath);
}
