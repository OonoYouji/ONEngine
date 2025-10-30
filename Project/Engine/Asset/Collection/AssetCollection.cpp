#include "AssetCollection.h"

/// std
#include <filesystem>

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Core/Utility/Math/Mathf.h"

AssetCollection::AssetCollection() {}
AssetCollection::~AssetCollection() {}


void AssetCollection::Initialize(DxManager* _dxm) {
	assetLoader_ = std::make_unique<AssetLoader>(_dxm, this);
	assetLoader_->Initialize();

	/// リソースコンテナの初期化
	modelContainer_ = std::make_unique<AssetContainer<Model>>(static_cast<size_t>(MAX_MODEL_COUNT));
	textureContainer_ = std::make_unique<AssetContainer<Texture>>(static_cast<size_t>(MAX_TEXTURE_COUNT));
	audioClipContainer_ = std::make_unique<AssetContainer<AudioClip>>(static_cast<size_t>(MAX_AUDIOCLIP_COUNT));
	materialContainer_ = std::make_unique<AssetContainer<Material>>(static_cast<size_t>(MAX_MATERIAL_COUNT));

	RegisterResourceType();

	/// Packages内のファイルがすべて読み込む
	LoadResources(GetResourceFilePaths("./Packages/"));
	LoadResources(GetResourceFilePaths("./Assets/"));
}

void AssetCollection::LoadResources(const std::vector<std::string>& _filePaths) {
	/// ----- 引数のファイルパス群をループしてリソースを読み込む ----- ///

	std::string extension;
	for (auto& path : _filePaths) {
		AssetType type = AssetType::None;

		/// 拡張子をチェックして、リソースの種類を決定
		extension = Mathf::FileExtension(path);
		if (resourceTypes_.contains(extension)) {
			type = resourceTypes_[extension];
			Load(path, type);
		}
	}

}

void AssetCollection::UnloadResources(const std::vector<std::string>& _filePaths) {
	/// ----- 引数のファイルパス群をループしてリソースを解放する ----- ///

	std::string extension;
	for (auto& path : _filePaths) {
		AssetType type = AssetType::None;

		/// 拡張子をチェックして、リソースの種類を決定
		extension = Mathf::FileExtension(path);
		if (resourceTypes_.contains(extension)) {
			type = resourceTypes_[extension];
		}

		switch (type) {
		case AssetType::None:
			continue; ///< noneの場合は何もしない
		case AssetType::Texture:
			break;
		case AssetType::Mesh:
			/// meshの解放
			modelContainer_->Remove(path);
			break;
		}

	}

}

void AssetCollection::Load(const std::string& _filepath, AssetType _type) {
	/// ----- 指定されたファイルパスのリソースを読み込む ----- ///

	///< noneの場合は読み込まない
	if (_type == AssetType::None) {
		return;
	}

	switch (_type) {
	case AssetType::Texture:
		/// 読み込み済みかチェックし、読み込んでいない場合のみ読み込む
		if (GetTexture(_filepath) == nullptr) {
			assetLoader_->LoadTexture(_filepath);
		}
		break;
	case AssetType::Mesh:
		assetLoader_->LoadModelObj(_filepath);
		break;
	case AssetType::Audio:
		assetLoader_->LoadAudioClip(_filepath);
		break;
	case AssetType::Material:
		assetLoader_->LoadMaterial(_filepath);
		break;
	}

}

void AssetCollection::HotReload(const std::string& _filepath) {
	/// ----- 指定されたファイルパスのリソースをホットリロードする ----- ///

	/// ファイルの拡張子を取得
	const std::string extension = Mathf::FileNameWithoutExtension(_filepath);
	AssetType type = AssetType::None;
	/// 拡張子をチェックして、リソースの種類を決定
	if (resourceTypes_.contains(extension)) {
		type = resourceTypes_[extension];
	} else {
		Console::LogWarning("Unsupported file type for hot reload: " + _filepath);
		return;
	}


	switch (type) {
	case AssetType::Texture:
		/// テクスチャの再読み込み
		assetLoader_->LoadTexture(_filepath);
		break;
	case AssetType::Mesh:
		/// モデルの再読み込み
		assetLoader_->LoadModelObj(_filepath);
		break;
	}
}

void AssetCollection::HotReloadAll() {
	/// ----- すべてのリソースをホットリロードする ----- ///

	for (const auto& model : modelContainer_->GetIndexMap()) {
		assetLoader_->LoadModelObj(model.first);
	}

	for (const auto& texture : textureContainer_->GetIndexMap()) {
		assetLoader_->LoadTexture(texture.first);
	}
}

/// AddAssetのテンプレート実装
template<>
void AssetCollection::AddAsset<Model>(const std::string& _filepath, Model&& _asset) {
	modelContainer_->Add(_filepath, std::move(_asset));
}

template<>
void AssetCollection::AddAsset<Texture>(const std::string& _filepath, Texture&& _asset) {
	textureContainer_->Add(_filepath, std::move(_asset));
}

template<>
void AssetCollection::AddAsset<AudioClip>(const std::string& _filepath, AudioClip&& _asset) {
	audioClipContainer_->Add(_filepath, std::move(_asset));
}

template<>
void AssetCollection::AddAsset<Material>(const std::string& _filepath, Material&& _asset) {
	materialContainer_->Add(_filepath, std::move(_asset));
}


bool AssetCollection::IsAssetExist(const Guid& _guid) const {
	/// ----- guidがアセットの物かチェックする ----- ///

	/// 無効値ならfalseを返す
	if (!_guid.CheckValid()) {
		return false;
	}

	/// 各コンテナでguidをチェック
	if (modelContainer_->GetIndex(_guid) != -1) {
		return true;
	}

	if (textureContainer_->GetIndex(_guid) != -1) {
		return true;
	}

	if (audioClipContainer_->GetIndex(_guid) != -1) {
		return true;
	}

	if (materialContainer_->GetIndex(_guid) != -1) {
		return true;
	}

	/// どのコンテナにも存在しなかった場合はfalseを返す
	return false;
}

bool AssetCollection::HasAsset(const std::string& _filepath) {
	/// ----- アセットを持っているのかチェックする ----- ///

	/// Typeの判定
	const std::string extension = Mathf::FileExtension(_filepath);
	AssetType type = GetAssetTypeFromExtension(extension);

	/// 各コンテナで存在チェック
	switch (type) {
	case AssetType::Texture:
		/// Textureの存在チェック
		if (textureContainer_->GetIndex(_filepath) != -1) {
			return true;
		}
		break;
	case AssetType::Mesh:
		/// Mesh
		if (modelContainer_->GetIndex(_filepath) != -1) {
			return true;
		}
		break;
	case AssetType::Audio:
		/// Audio
		if (audioClipContainer_->GetIndex(_filepath) != -1) {
			return true;
		}
		break;
	case AssetType::Material:
		/// Material
		if (materialContainer_->GetIndex(_filepath) != -1) {
			return true;
		}
		break;
	}


	return false;
}

bool AssetCollection::ReloadAsset(const std::string& _filepath) {
	/// ----- アセットのリロード ----- ///

	/// Typeの判定
	const std::string extension = Mathf::FileExtension(_filepath);
	AssetType type = GetAssetTypeFromExtension(extension);

	/// Typeごとにリロード処理を実行
	switch (type) {
	case AssetType::Texture:
		/// Textureのリロード
		if (assetLoader_->LoadTexture(_filepath)) {
			return true;
		}
		break;
	case AssetType::Mesh:
		/// Meshのリロード
		if (assetLoader_->LoadModelObj(_filepath)) {
			return true;
		}
		break;
	case AssetType::Audio:
		/// Audioのリロード
		if (assetLoader_->LoadAudioClip(_filepath)) {
			return true;
		}
		break;
	case AssetType::Material:
		/// Materialのリロード
		assetLoader_->LoadMaterial(_filepath);
		break;
	}

	return false;
}

std::vector<std::string> AssetCollection::GetResourceFilePaths(const std::string& _directoryPath) const {
	/// ----- 指定されたディレクトリパス内のリソースファイルパスをすべて取得する ----- ///

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
	resourceTypes_[".png"] = AssetType::Texture;
	resourceTypes_[".jpg"] = AssetType::Texture;
	resourceTypes_[".dds"] = AssetType::Texture;
	resourceTypes_[".obj"] = AssetType::Mesh;
	resourceTypes_[".gltf"] = AssetType::Mesh;
	resourceTypes_[".wav"] = AssetType::Audio;
	resourceTypes_[".mp3"] = AssetType::Audio;
	resourceTypes_[".mat"] = AssetType::Material;
}

const Guid& AssetCollection::GetAssetGuidFromPath(const std::string& _filepath) const {
	const std::string extension = Mathf::FileExtension(_filepath);
	AssetType type = AssetType::None;
	/// 拡張子をチェックして、リソースの種類を決定
	if (resourceTypes_.contains(extension)) {
		type = resourceTypes_.at(extension);
	}

	switch (type) {
	case AssetType::Texture:
		return textureContainer_->GetGuid(_filepath);
	case AssetType::Mesh:
		return modelContainer_->GetGuid(_filepath);
	case AssetType::Audio:
		return audioClipContainer_->GetGuid(_filepath);
	case AssetType::Material:
		return materialContainer_->GetGuid(_filepath);
	}

	return Guid::kInvalid;
}

AssetType AssetCollection::GetAssetTypeFromGuid(const Guid& _guid) const {
	/// ----- GuidからAssetTypeを取得する ----- ///

	if (modelContainer_->GetIndex(_guid) != -1) {
		return AssetType::Mesh;
	}

	if (textureContainer_->GetIndex(_guid) != -1) {
		return AssetType::Texture;
	}

	if (audioClipContainer_->GetIndex(_guid) != -1) {
		return AssetType::Audio;
	}

	if (materialContainer_->GetIndex(_guid) != -1) {
		return AssetType::Material;
	}

	return AssetType::None;
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

const std::string& AssetCollection::GetTexturePath(const Guid& _guid) const {
	/// Guid -> Index -> Path の順番で取得
	return textureContainer_->GetKey(textureContainer_->GetIndex(_guid));
}

Texture* AssetCollection::GetTextureFromGuid(const Guid& _guid) const {
	/// 無効値ならnullptrを返す
	if (!_guid.CheckValid()) {
		return nullptr;
	}

	return textureContainer_->Get(textureContainer_->GetIndex(_guid));
}

const AudioClip* AssetCollection::GetAudioClip(const std::string& _filepath) const {
	return audioClipContainer_->Get(_filepath);
}

AudioClip* AssetCollection::GetAudioClip(const std::string& _filepath) {
	return audioClipContainer_->Get(_filepath);
}
