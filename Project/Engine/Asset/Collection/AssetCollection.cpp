#include "AssetCollection.h"

/// std
#include <filesystem>

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Core/Threading/ThreadPool.h"

namespace ONEngine::Asset {


AssetCollection::AssetCollection() = default;
AssetCollection::~AssetCollection() = default;

void AssetCollection::Initialize(DxManager* dxm) {

	const size_t assetTypeCount = static_cast<size_t>(AssetType::Count);
	assetBundles_.resize(assetTypeCount);

	// 基底クラス(IAssetBundle等)のポインタとして保持し、実体を作成
	assetBundles_[static_cast<size_t>(AssetType::Mesh)] = std::make_unique<AssetBundle<Model>>();
	assetBundles_[static_cast<size_t>(AssetType::Texture)] = std::make_unique<AssetBundle<Texture>>();
	assetBundles_[static_cast<size_t>(AssetType::Audio)] = std::make_unique<AssetBundle<AudioClip>>();
	assetBundles_[static_cast<size_t>(AssetType::Material)] = std::make_unique<AssetBundle<Material>>();
	assetBundles_[static_cast<size_t>(AssetType::Shader)] = std::make_unique<AssetBundle<Shader>>();

	// ヘルパーを使ってセットアップ（キャスト記述が減りスマートになります）
	auto* meshBundle = GetBundle<Model>(AssetType::Mesh);
	meshBundle->loader = std::make_unique<AssetLoader<Model>>(dxm);
	meshBundle->container = std::make_unique<AssetContainer<Model>>(MAX_MODEL_COUNT);

	auto* textureBundle = GetBundle<Texture>(AssetType::Texture);
	textureBundle->loader = std::make_unique<AssetLoader<Texture>>(dxm, this);
	textureBundle->container = std::make_unique<AssetContainer<Texture>>(MAX_TEXTURE_COUNT);

	auto* audioBundle = GetBundle<AudioClip>(AssetType::Audio);
	audioBundle->loader = std::make_unique<AssetLoader<AudioClip>>();
	audioBundle->container = std::make_unique<AssetContainer<AudioClip>>(MAX_AUDIOCLIP_COUNT);

	auto* materialBundle = GetBundle<Material>(AssetType::Material);
	materialBundle->loader = std::make_unique<AssetLoader<Material>>();
	materialBundle->container = std::make_unique<AssetContainer<Material>>(MAX_MATERIAL_COUNT);

	auto* shaderBundle = GetBundle<Shader>(AssetType::Shader);
	shaderBundle->loader = std::make_unique<AssetLoader<Shader>>();
	shaderBundle->container = std::make_unique<AssetContainer<Shader>>(128); // 適当な数

	LoadResourcesAsync(GetResourceFilePaths("./Packages/"));
	LoadResourcesAsync(GetResourceFilePaths("./Assets/"));

	WaitAllLoads();
}

void AssetCollection::LoadResources(const std::vector<std::string>& filePaths) {
	for(auto& path : filePaths) {
		AssetType type = GetAssetTypeFromExtension(FileSystem::FileExtension(path));
		if(type != AssetType::None) {
			Load(path, type);
		}
	}
}


///
/// アセットの非同期読み込み
///
void AssetCollection::LoadResourcesAsync(const std::vector<std::string>& filePaths) {
	for(auto& path : filePaths) {

		/// 有効なアセットタイプかチェック
		AssetType type = GetAssetTypeFromExtension(FileSystem::FileExtension(path));
		if(type != AssetType::None && type != AssetType::Count) {

			/// 非同期タスクをスレッドプールに投げる
			if(auto* bundle = GetBaseBundle(type)) {
				auto future = ThreadPool::Instance().Enqueue([bundle, path]() {
					bundle->Load(path);
				});

				pendingTasks_.push_back(std::move(future));
			}
		}

	}
}

/// [非同期化による追加] 投げた非同期ロードがすべて終わるまで待機
void AssetCollection::WaitAllLoads() {
	for(auto& task : pendingTasks_) {
		if(task.valid()) {
			task.wait();
		}
	}
	// 待機が完了したらリストをクリアする
	pendingTasks_.clear();
}

void AssetCollection::UnloadResources(const std::vector<std::string>& filePaths) {
	for(auto& path : filePaths) {
		UnloadAssetByPath(path);
	}
}

void AssetCollection::UnloadAssetByPath(const std::string& filepath) {
	/// アセットの削除
	const std::string extension = FileSystem::FileExtension(filepath);
	const AssetType type = GetAssetTypeFromExtension(extension);
	if(auto* bundle = GetBaseBundle(type)) {
		bundle->Remove(filepath);
	}
}

void AssetCollection::Load(const std::string& filepath, AssetType type) {
	if(auto* bundle = GetBaseBundle(type)) {
		bundle->Load(filepath);
	}
}

/// AddAssetのテンプレート実装
template<>
void AssetCollection::AddAsset<Model>(const std::string& filepath, Model&& asset) {
	GetBundle<Model>(AssetType::Mesh)->container->Add(filepath, std::move(asset));
}

template<>
void AssetCollection::AddAsset<Texture>(const std::string& filepath, Texture&& asset) {
	GetBundle<Texture>(AssetType::Texture)->container->Add(filepath, std::move(asset));
}

template<>
void AssetCollection::AddAsset<AudioClip>(const std::string& filepath, AudioClip&& asset) {
	GetBundle<AudioClip>(AssetType::Audio)->container->Add(filepath, std::move(asset));
}

template<>
void AssetCollection::AddAsset<Material>(const std::string& filepath, Material&& asset) {
	GetBundle<Material>(AssetType::Material)->container->Add(filepath, std::move(asset));
}

bool AssetCollection::IsAsset(const Guid& guid) const {
	if(!guid.CheckValid()) {
		return false;
	}

	for(const auto& bundle : assetBundles_) {
		if(bundle && bundle->Contains(guid)) {
			return true;
		}
	}
	return false;
}

bool AssetCollection::HasAsset(const std::string& filepath) {
	const std::string extension = FileSystem::FileExtension(filepath);
	AssetType type = GetAssetTypeFromExtension(extension);

	if(auto* bundle = GetBaseBundle(type)) {
		return bundle->Contains(filepath);
	}

	return false;
}

bool AssetCollection::ReloadAsset(const std::string& filepath) {
	const std::string extension = FileSystem::FileExtension(filepath);
	AssetType type = GetAssetTypeFromExtension(extension);

	if(auto* bundle = GetBaseBundle(type)) {
		bundle->Reload(filepath);
	}

	return true;
}

std::vector<std::string> AssetCollection::GetResourceFilePaths(const std::string& directoryPath) const {
	std::vector<std::string> resourcePaths;
	for(const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath)) {
		if(entry.is_regular_file()) {
			std::string path = entry.path().string();
			FileSystem::ReplaceAll(&path, "\\", "/");
			AssetType type = GetAssetTypeFromExtension(FileSystem::FileExtension(path));
			if(type != AssetType::None) {
				resourcePaths.push_back(path);
			}
		}
	}
	return resourcePaths;
}

IAssetBundle* ONEngine::Asset::AssetCollection::GetBaseBundle(Asset::AssetType type) const {
	if(type == AssetType::None) {
		return nullptr;
	}

	size_t index = static_cast<size_t>(type);
	if(index >= assetBundles_.size() || !assetBundles_[index]) {
		return nullptr;
	}

	return assetBundles_[index].get();
}

const Guid& AssetCollection::GetAssetGuidFromPath(const std::string& filepath) const {
	const std::string extension = FileSystem::FileExtension(filepath);
	AssetType type = GetAssetTypeFromExtension(extension);

	if(auto* bundle = GetBaseBundle(type)) {
		return bundle->GetGuid(filepath);
	}

	return Guid::kInvalid;
}

AssetType AssetCollection::GetAssetTypeFromGuid(const Guid& guid) const {
	if(!guid.CheckValid()) {
		return AssetType::None;
	}

	for(size_t i = 0; i < assetBundles_.size(); ++i) {
		if(assetBundles_[i] && assetBundles_[i]->Contains(guid)) {
			return static_cast<AssetType>(i);
		}
	}

	return AssetType::None;
}

const Model* AssetCollection::GetModel(const std::string& filepath) const {
	return GetBundle<Model>(AssetType::Mesh)->container->Get(filepath);
}

Model* AssetCollection::GetModel(const std::string& filepath) {
	return GetBundle<Model>(AssetType::Mesh)->container->Get(filepath);
}

const Texture* AssetCollection::GetTexture(const std::string& filepath) const {
	return GetBundle<Texture>(AssetType::Texture)->container->Get(filepath);
}

Texture* AssetCollection::GetTexture(const std::string& filepath) {
	return GetBundle<Texture>(AssetType::Texture)->container->Get(filepath);
}

int32_t AssetCollection::GetTextureIndex(const std::string& filepath) const {
	return GetBundle<Texture>(AssetType::Texture)->container->GetIndex(filepath);
}

const std::string& AssetCollection::GetTexturePath(size_t index) const {
	return GetBundle<Texture>(AssetType::Texture)->container->GetKey(static_cast<int32_t>(index));
}

const std::vector<Texture>& AssetCollection::GetTextures() const {
	return GetBundle<Texture>(AssetType::Texture)->container->GetValues();
}

int32_t AssetCollection::GetTextureIndexFromGuid(const Guid& guid) const {
	return GetBundle<Texture>(AssetType::Texture)->container->GetIndex(guid);
}

const std::string& AssetCollection::GetTexturePath(const Guid& guid) const {
	auto* container = GetBundle<Texture>(AssetType::Texture)->container.get();
	return container->GetKey(container->GetIndex(guid));
}

Texture* AssetCollection::GetTextureFromGuid(const Guid& guid) const {
	if(!guid.CheckValid()) return nullptr;
	auto* container = GetBundle<Texture>(AssetType::Texture)->container.get();
	return container->Get(container->GetIndex(guid));
}

const AudioClip* AssetCollection::GetAudioClip(const std::string& filepath) const {
	return GetBundle<AudioClip>(AssetType::Audio)->container->Get(filepath);
}

AudioClip* AssetCollection::GetAudioClip(const std::string& filepath) {
	return GetBundle<AudioClip>(AssetType::Audio)->container->Get(filepath);
}


} /// namespace ONEngine