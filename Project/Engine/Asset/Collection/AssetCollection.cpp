#include "AssetCollection.h"

/// std
#include <filesystem>

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Core/Utility/Math/Mathf.h"

AssetCollection::AssetCollection() {}
AssetCollection::~AssetCollection() {}


void AssetCollection::Initialize(DxManager* _dxManager) {
	resourceLoader_ = std::make_unique<GraphicsResourceLoader>(_dxManager, this);
	resourceLoader_->Initialize();

	modelContainer_ = std::make_unique<ResourceContainer<Model>>(static_cast<size_t>(MAX_MODEL_COUNT));
	textureContainer_ = std::make_unique<ResourceContainer<Texture>>(static_cast<size_t>(MAX_TEXTURE_COUNT));
	audioClipContainer_ = std::make_unique<ResourceContainer<AudioClip>>(static_cast<size_t>(MAX_AUDIOCLIP_COUNT));

	RegisterResourceType();

	/// Packages内のファイルがすべて読み込む
	LoadResources(GetResourceFilePaths("./Packages/"));
	LoadResources(GetResourceFilePaths("./Assets/"));
}

void AssetCollection::LoadResources(const std::vector<std::string>& _filePaths) {

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

void AssetCollection::UnloadResources(const std::vector<std::string>& _filePaths) {

	std::string extension;
	for (auto& path : _filePaths) {
		Type type = Type::none;

		/// 拡張子をチェックして、リソースの種類を決定
		extension = Mathf::FileExtension(path);
		if (resourceTypes_.contains(extension)) {
			type = resourceTypes_[extension];
		}

		switch (type) {
		case AssetCollection::Type::none:
			continue; ///< noneの場合は何もしない
		case AssetCollection::Type::texture:
			break;
		case AssetCollection::Type::model:
			/// meshの解放
			modelContainer_->Remove(path);
			break;
		}

	}

}

void AssetCollection::Load(const std::string& _filepath, Type _type) {
	///< noneの場合は読み込まない
	if (_type == Type::none) {
		return;
	}

	switch (_type) {
	case Type::texture:
		/// 読み込み済みかチェックし、読み込んでいない場合のみ読み込む
		if (GetTexture(_filepath) == nullptr) {
			resourceLoader_->LoadTexture(_filepath);
		}
		break;
	case Type::model:
		resourceLoader_->LoadModelObj(_filepath);
		break;
	case Type::audio:
		resourceLoader_->LoadAudioClip(_filepath);
		break;
	}

}

void AssetCollection::HotReload(const std::string& _filepath) {
	/// ファイルの拡張子を取得
	const std::string extension = Mathf::FileNameWithoutExtension(_filepath);
	Type type = Type::none;
	/// 拡張子をチェックして、リソースの種類を決定
	if (resourceTypes_.contains(extension)) {
		type = resourceTypes_[extension];
	} else {
		Console::LogWarning("Unsupported file type for hot reload: " + _filepath);
		return;
	}


	switch (type) {
	case AssetCollection::Type::texture:
		/// テクスチャの再読み込み
		resourceLoader_->LoadTexture(_filepath);
		break;
	case AssetCollection::Type::model:
		/// モデルの再読み込み
		resourceLoader_->LoadModelObj(_filepath);
		break;
	}
}

void AssetCollection::HotReloadAll() {
	for (const auto& model : modelContainer_->GetIndexMap()) {
		resourceLoader_->LoadModelObj(model.first);
	}

	for (const auto& texture : textureContainer_->GetIndexMap()) {
		resourceLoader_->LoadTexture(texture.first);
	}
}

void AssetCollection::AddModel(const std::string& _filepath, Model&& _model) {
	modelContainer_->Add(_filepath, _model);
}

void AssetCollection::AddTexture(const std::string& _filepath, Texture&& _texture) {
	_texture.SetName(_filepath);
	textureContainer_->Add(_filepath, _texture);
}

void AssetCollection::AddAudioClip(const std::string& _filepath, AudioClip&& _audioClip) {
	audioClipContainer_->Add(_filepath, std::move(_audioClip));
}

std::vector<std::string> AssetCollection::GetResourceFilePaths(const std::string& _directoryPath) const {
	std::vector<std::string> resourcePaths;

	for (const auto& entry : std::filesystem::recursive_directory_iterator(_directoryPath)) {
		if (entry.is_regular_file()) {
			std::string path = entry.path().string();
			/// パスの中にある "\\" を "/" に置き換える
			Mathf::ReplaceAll(&path, "\\", "/");

			/// 拡張子をチェックして、リソースの種類を決定
			if (resourceTypes_.contains(Mathf::FileExtension(path))) {
				resourcePaths.push_back(path);
			}

		}
	}

	return resourcePaths;
}

void AssetCollection::RegisterResourceType() {
	/// リソースの種類を登録
	resourceTypes_[".png"] = Type::texture;
	resourceTypes_[".jpg"] = Type::texture;
	resourceTypes_[".dds"] = Type::texture;
	resourceTypes_[".obj"] = Type::model;
	resourceTypes_[".gltf"] = Type::model;
	resourceTypes_[".wav"] = Type::audio;
	resourceTypes_[".mp3"] = Type::audio;
}

const Model* AssetCollection::GetModel(const std::string& _filepath) const {
	return modelContainer_->Get(_filepath);
}

Model* AssetCollection::GetModel(const std::string& _filepath) {
	return modelContainer_->Get(_filepath);
}

const Texture* AssetCollection::GetTexture(const std::string& _filepath) const {
	return textureContainer_->Get(_filepath);
}

Texture* AssetCollection::GetTexture(const std::string& _filepath) {
	return textureContainer_->Get(_filepath);
}

size_t AssetCollection::GetTextureIndex(const std::string& _filepath) const {
	return textureContainer_->GetIndex(_filepath);
}

const std::string& AssetCollection::GetTexturePath(size_t _index) const {
	return textureContainer_->GetKey(static_cast<int32_t>(_index));
}

const std::vector<Texture>& AssetCollection::GetTextures() const {
	return textureContainer_->GetValues();
}

int32_t AssetCollection::GetTextureIndexFromGuid(const Guid& _guid) const {
	return textureContainer_->GetIndex(_guid);
}

const AudioClip* AssetCollection::GetAudioClip(const std::string& _filepath) const {
	return audioClipContainer_->Get(_filepath);
}

AudioClip* AssetCollection::GetAudioClip(const std::string& _filepath) {
	return audioClipContainer_->Get(_filepath);
}
