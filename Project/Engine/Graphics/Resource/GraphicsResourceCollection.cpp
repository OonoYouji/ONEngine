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
			modelContainer_->Remove(path);
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
	for (const auto& model : modelContainer_->GetIndexMap()) {
		resourceLoader_->LoadModelObj(model.first);
	}

	for (const auto& texture : textureContainer_->GetIndexMap()) {
		resourceLoader_->LoadTexture(texture.first);
	}
}

void GraphicsResourceCollection::AddModel(const std::string& _filePath, Model&& _model) {
	modelContainer_->Add(_filePath, _model);
}

void GraphicsResourceCollection::AddTexture(const std::string& _filePath, Texture&& _texture) {
	_texture.SetName(_filePath);
	textureContainer_->Add(_filePath, _texture);
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
	return modelContainer_->Get(_filePath);
}

Model* GraphicsResourceCollection::GetModel(const std::string& _filePath) {
	return modelContainer_->Get(_filePath);
}

const Texture* GraphicsResourceCollection::GetTexture(const std::string& _filePath) const {
	return textureContainer_->Get(_filePath);
}

Texture* GraphicsResourceCollection::GetTexture(const std::string& _filePath) {
	return textureContainer_->Get(_filePath);
}

size_t GraphicsResourceCollection::GetTextureIndex(const std::string& _filePath) const {
	return textureContainer_->GetIndex(_filePath);
}

const std::string& GraphicsResourceCollection::GetTexturePath(size_t _index) const {
	return textureContainer_->GetKey(_index);
}

const std::vector<Texture>& GraphicsResourceCollection::GetTextures() const {
	return textureContainer_->GetValues();
}
