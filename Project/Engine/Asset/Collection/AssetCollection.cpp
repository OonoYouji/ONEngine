#include "AssetCollection.h"

/// std
#include <filesystem>

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Core/Utility/Math/Math.h"

using namespace ONEngine;

AssetCollection::AssetCollection() = default;
AssetCollection::~AssetCollection() = default;



void AssetCollection::Initialize(DxManager* _dxm) {

	const size_t assetTypeCount = static_cast<size_t>(AssetType::Count);
	assetBundles_.resize(assetTypeCount);

	// 基底クラス(IAssetBundle等)のポインタとして保持し、実体を作成
	assetBundles_[static_cast<size_t>(AssetType::Mesh)] = std::make_unique<AssetBundle<Model>>();
	assetBundles_[static_cast<size_t>(AssetType::Texture)] = std::make_unique<AssetBundle<Texture>>();
	assetBundles_[static_cast<size_t>(AssetType::Audio)] = std::make_unique<AssetBundle<AudioClip>>();
	assetBundles_[static_cast<size_t>(AssetType::Material)] = std::make_unique<AssetBundle<Material>>();

	// ヘルパーを使ってセットアップ（キャスト記述が減りスマートになります）
	auto* meshBundle = GetBundle<Model>(AssetType::Mesh);
	meshBundle->loader = std::make_unique<AssetLoader<Model>>(_dxm);
	meshBundle->container = std::make_unique<AssetContainer<Model>>(MAX_MODEL_COUNT);

	auto* textureBundle = GetBundle<Texture>(AssetType::Texture);
	textureBundle->loader = std::make_unique<AssetLoader<Texture>>(_dxm, this);
	textureBundle->container = std::make_unique<AssetContainer<Texture>>(MAX_TEXTURE_COUNT);

	auto* audioBundle = GetBundle<AudioClip>(AssetType::Audio);
	audioBundle->loader = std::make_unique<AssetLoader<AudioClip>>();
	audioBundle->container = std::make_unique<AssetContainer<AudioClip>>(MAX_AUDIOCLIP_COUNT);

	auto* materialBundle = GetBundle<Material>(AssetType::Material);
	materialBundle->loader = std::make_unique<AssetLoader<Material>>();
	materialBundle->container = std::make_unique<AssetContainer<Material>>(MAX_MATERIAL_COUNT);

	/// Packages内のファイルがすべて読み込む
	LoadResources(GetResourceFilePaths("./Packages/"));
	LoadResources(GetResourceFilePaths("./Assets/"));
}

void AssetCollection::LoadResources(const std::vector<std::string>& _filePaths) {
	for (auto& path : _filePaths) {
		AssetType type = GetAssetTypeFromExtension(FileSystem::FileExtension(path));
		if (type != AssetType::None) {
			Load(path, type);
		}
	}
}

void AssetCollection::UnloadResources(const std::vector<std::string>& _filePaths) {
	for (auto& path : _filePaths) {
		UnloadAssetByPath(path);
	}
}

void AssetCollection::UnloadAssetByPath(const std::string& _filepath) {
	/// アセットの削除
	const std::string extension = FileSystem::FileExtension(_filepath);
	const AssetType type = GetAssetTypeFromExtension(extension);
	if (auto* bundle = GetBaseBundle(type)) {
		bundle->Remove(_filepath);
	}
}

void AssetCollection::Load(const std::string& _filepath, AssetType _type) {
	if (auto* bundle = GetBaseBundle(_type)) {
		bundle->Load(_filepath);
	}
}

/// AddAssetのテンプレート実装
template<>
void AssetCollection::AddAsset<Model>(const std::string& _filepath, Model&& _asset) {
	GetBundle<Model>(AssetType::Mesh)->container->Add(_filepath, std::move(_asset));
}

template<>
void AssetCollection::AddAsset<Texture>(const std::string& _filepath, Texture&& _asset) {
	GetBundle<Texture>(AssetType::Texture)->container->Add(_filepath, std::move(_asset));
}

template<>
void AssetCollection::AddAsset<AudioClip>(const std::string& _filepath, AudioClip&& _asset) {
	GetBundle<AudioClip>(AssetType::Audio)->container->Add(_filepath, std::move(_asset));
}

template<>
void AssetCollection::AddAsset<Material>(const std::string& _filepath, Material&& _asset) {
	GetBundle<Material>(AssetType::Material)->container->Add(_filepath, std::move(_asset));
}

bool AssetCollection::IsAsset(const Guid& _guid) const {
	if (!_guid.CheckValid()) {
		return false;
	}

	for (const auto& bundle : assetBundles_) {
		if (bundle && bundle->Contains(_guid)) {
			return true;
		}
	}
	return false;
}

bool AssetCollection::HasAsset(const std::string& _filepath) {
	const std::string extension = FileSystem::FileExtension(_filepath);
	AssetType type = GetAssetTypeFromExtension(extension);

	if (auto* bundle = GetBaseBundle(type)) {
		return bundle->Contains(_filepath);
	}

	return false;
}

bool AssetCollection::ReloadAsset(const std::string& _filepath) {
	const std::string extension = FileSystem::FileExtension(_filepath);
	AssetType type = GetAssetTypeFromExtension(extension);

	if (auto* bundle = GetBaseBundle(type)) {
		bundle->Reload(_filepath);
	}

	return true;
}

std::vector<std::string> AssetCollection::GetResourceFilePaths(const std::string& _directoryPath) const {
	std::vector<std::string> resourcePaths;
	for (const auto& entry : std::filesystem::recursive_directory_iterator(_directoryPath)) {
		if (entry.is_regular_file()) {
			std::string path = entry.path().string();
			FileSystem::ReplaceAll(&path, "\\", "/");
			AssetType type = GetAssetTypeFromExtension(FileSystem::FileExtension(path));
			if (type != AssetType::None) {
				resourcePaths.push_back(path);
			}
		}
	}
	return resourcePaths;
}

IAssetBundle* ONEngine::AssetCollection::GetBaseBundle(AssetType _type) const {
	if (_type == AssetType::None) {
		return nullptr;
	}

	size_t index = static_cast<size_t>(_type);
	if (index >= assetBundles_.size() || !assetBundles_[index]) {
		return nullptr;
	}

	return assetBundles_[index].get();
}

const Guid& AssetCollection::GetAssetGuidFromPath(const std::string& _filepath) const {
	const std::string extension = FileSystem::FileExtension(_filepath);
	AssetType type = GetAssetTypeFromExtension(extension);

	if (auto* bundle = GetBaseBundle(type)) {
		return bundle->GetGuid(_filepath);
	}

	return Guid::kInvalid;
}

AssetType AssetCollection::GetAssetTypeFromGuid(const Guid& _guid) const {
	if (!_guid.CheckValid()) {
		return AssetType::None;
	}

	for (size_t i = 0; i < assetBundles_.size(); ++i) {
		if (assetBundles_[i] && assetBundles_[i]->Contains(_guid)) {
			return static_cast<AssetType>(i);
		}
	}

	return AssetType::None;
}

const Model* AssetCollection::GetModel(const std::string& _filepath) const {
	return GetBundle<Model>(AssetType::Mesh)->container->Get(_filepath);
}

Model* AssetCollection::GetModel(const std::string& _filepath) {
	return GetBundle<Model>(AssetType::Mesh)->container->Get(_filepath);
}

const Texture* AssetCollection::GetTexture(const std::string& _filepath) const {
	return GetBundle<Texture>(AssetType::Texture)->container->Get(_filepath);
}

Texture* AssetCollection::GetTexture(const std::string& _filepath) {
	return GetBundle<Texture>(AssetType::Texture)->container->Get(_filepath);
}

size_t AssetCollection::GetTextureIndex(const std::string& _filepath) const {
	return GetBundle<Texture>(AssetType::Texture)->container->GetIndex(_filepath);
}

const std::string& AssetCollection::GetTexturePath(size_t _index) const {
	return GetBundle<Texture>(AssetType::Texture)->container->GetKey(static_cast<int32_t>(_index));
}

const std::vector<Texture>& AssetCollection::GetTextures() const {
	return GetBundle<Texture>(AssetType::Texture)->container->GetValues();
}

int32_t AssetCollection::GetTextureIndexFromGuid(const Guid& _guid) const {
	return GetBundle<Texture>(AssetType::Texture)->container->GetIndex(_guid);
}

const std::string& AssetCollection::GetTexturePath(const Guid& _guid) const {
	auto* container = GetBundle<Texture>(AssetType::Texture)->container.get();
	return container->GetKey(container->GetIndex(_guid));
}

Texture* AssetCollection::GetTextureFromGuid(const Guid& _guid) const {
	if (!_guid.CheckValid()) return nullptr;
	auto* container = GetBundle<Texture>(AssetType::Texture)->container.get();
	return container->Get(container->GetIndex(_guid));
}

const AudioClip* AssetCollection::GetAudioClip(const std::string& _filepath) const {
	return GetBundle<AudioClip>(AssetType::Audio)->container->Get(_filepath);
}

AudioClip* AssetCollection::GetAudioClip(const std::string& _filepath) {
	return GetBundle<AudioClip>(AssetType::Audio)->container->Get(_filepath);
}

