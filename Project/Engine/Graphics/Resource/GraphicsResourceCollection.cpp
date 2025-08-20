#include "GraphicsResourceCollection.h"

/// std
#include <filesystem>

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Creator/PrimitiveMeshCreator.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Core/Utility/Math/Mathf.h"

GraphicsResourceCollection::GraphicsResourceCollection() {}
GraphicsResourceCollection::~GraphicsResourceCollection() {}


void GraphicsResourceCollection::Initialize(DxManager* _dxManager) {
	resourceLoader_ = std::make_unique<GraphicsResourceLoader>(_dxManager, this);
	resourceLoader_->Initialize();

	modelContainer_ = std::make_unique<ModelContainer>(static_cast<size_t>(MAX_MODEL_COUNT));
	textureContainer_ = std::make_unique<TextureContainer>(static_cast<size_t>(MAX_TEXTURE_COUNT));

	RegisterResourceType();

	textures_.resize(MAX_TEXTURE_COUNT);

	/// Packages内のファイルがすべて読み込む
	LoadResources(GetResourceFilePaths("./Packages/"));
	LoadResources(GetResourceFilePaths("./Assets/"));

	/// primitive meshを作成
	PrimitiveMeshCreator primitiveMeshCreator(this, _dxManager->GetDxDevice());
}

void GraphicsResourceCollection::LoadResources(const std::vector<std::string>& _filePaths) {

	std::string extension;
	for (auto& path : _filePaths) {
		Type type = Type::none;

		/// 拡張子をチェックして、リソースの種類を決定
		extension = Mathf::FileExtension(path);
		if (resourceTypes_.contains(extension)) {
			type = resourceTypes_[extension];
			Load(path, type);
		}
	}

}

void GraphicsResourceCollection::UnloadResources(const std::vector<std::string>& _filePaths) {

	std::string extension;
	for (auto& path : _filePaths) {
		Type type = Type::none;

		/// 拡張子をチェックして、リソースの種類を決定
		extension = Mathf::FileExtension(path);
		if (resourceTypes_.contains(extension)) {
			type = resourceTypes_[extension];
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

void GraphicsResourceCollection::HotReload(const std::string& _filePath) {
	/// ファイルの拡張子を取得
	const std::string extension = Mathf::FileNameWithoutExtension(_filePath);
	Type type = Type::none;
	/// 拡張子をチェックして、リソースの種類を決定
	if (resourceTypes_.contains(extension)) {
		type = resourceTypes_[extension];
	} else {
		Console::LogWarning("Unsupported file type for hot reload: " + _filePath);
		return;
	}


	switch (type) {
	case GraphicsResourceCollection::Type::texture:
		/// テクスチャの再読み込み
		resourceLoader_->LoadTexture(_filePath);
		break;
	case GraphicsResourceCollection::Type::model:
		/// モデルの再読み込み
		resourceLoader_->LoadModelObj(_filePath);
		break;
	}
}

void GraphicsResourceCollection::HotReloadAll() {
	for (const auto& model : models_) {
		resourceLoader_->LoadModelObj(model.first);
	}

	for (const auto& texture : textureIndices_) {
		resourceLoader_->LoadTexture(texture.first);
	}
}

void GraphicsResourceCollection::AddModel(const std::string& _filePath, std::unique_ptr<Model> _model) {
	models_[_filePath] = std::move(_model);
}

void GraphicsResourceCollection::AddTexture(const std::string& _filePath, std::unique_ptr<Texture> _texture) {
	_texture->SetName(_filePath);

	size_t value = textureIndices_.size();
	textureIndices_[_filePath] = value;
	reverseTextureIndices_[value] = _filePath;

	textures_[textureIndices_[_filePath]] = std::move(_texture);
}

std::vector<std::string> GraphicsResourceCollection::GetResourceFilePaths(const std::string& _directoryPath) const {
	std::vector<std::string> resourcePaths;


	for (const auto& entry : std::filesystem::recursive_directory_iterator(_directoryPath)) {
		if (entry.is_regular_file()) {
			std::string path = entry.path().string();
			/// パスの中にある "\\" を "/" に置き換える
			Mathf::ReplaceAll(&path, "\\", "/");

			/// 拡張子をチェックして、リソースの種類を決定
			if(resourceTypes_.contains(Mathf::FileExtension(path))) {
				resourcePaths.push_back(path);
			}


			//if (path.find(".png") != std::string::npos
			//	|| path.find(".jpg") != std::string::npos
			//	|| path.find(".dds") != std::string::npos
			//	|| path.find(".obj") != std::string::npos
			//	|| path.find(".gltf") != std::string::npos) {
			//	resourcePaths.push_back(path);
			//}
		}
	}

	return resourcePaths;
}

void GraphicsResourceCollection::RegisterResourceType() {
	/// リソースの種類を登録
	resourceTypes_[".png"] = Type::texture;
	resourceTypes_[".jpg"] = Type::texture;
	resourceTypes_[".dds"] = Type::texture;
	resourceTypes_[".obj"] = Type::model;
	resourceTypes_[".gltf"] = Type::model;
}

const Model* GraphicsResourceCollection::GetModel(const std::string& _filePath) const {
	auto itr = models_.find(_filePath);
	if (itr != models_.end()) {
		return itr->second.get();
	}

	return nullptr;
}

Model* GraphicsResourceCollection::GetModel(const std::string& _filePath) {
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

	if (textureIndices_.contains(_filePath) == false) {
		Console::Log("Texture not found: " + _filePath);
		return 0; // デフォルトのインデックスを返す
	}

	return textureIndices_.at(_filePath);
}

const std::string& GraphicsResourceCollection::GetTexturePath(size_t _index) const {
	if (reverseTextureIndices_.contains(_index)) {
		return reverseTextureIndices_.at(_index);
	}

	Console::LogError("Texture not found: " + std::to_string(_index));
	return "";
}

const std::vector<std::unique_ptr<Texture>>& GraphicsResourceCollection::GetTextures() const {
	return textures_;
}
