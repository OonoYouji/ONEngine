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
	modelLoader_ = std::make_unique<AssetLoaderT<Model>>(_dxm);
	textureLoader_ = std::make_unique<AssetLoaderT<Texture>>(_dxm, this);
	audioClipLoader_ = std::make_unique<AssetLoaderT<AudioClip>>();
	materialLoader_ = std::make_unique<AssetLoaderT<Material>>();


	//assetLoader_ = std::make_unique<AssetLoader>(_dxm, this);
	//assetLoader_->Initialize();

	/// リソースコンテナの初期化
	modelContainer_ = std::make_unique<AssetContainer<Model>>(static_cast<size_t>(MAX_MODEL_COUNT));
	textureContainer_ = std::make_unique<AssetContainer<Texture>>(static_cast<size_t>(MAX_TEXTURE_COUNT));
	audioClipContainer_ = std::make_unique<AssetContainer<AudioClip>>(static_cast<size_t>(MAX_AUDIOCLIP_COUNT));
	materialContainer_ = std::make_unique<AssetContainer<Material>>(static_cast<size_t>(MAX_MATERIAL_COUNT));

	/// Packages内のファイルがすべて読み込む
	LoadResources(GetResourceFilePaths("./Packages/"));
	LoadResources(GetResourceFilePaths("./Assets/"));
}

void AssetCollection::LoadResources(const std::vector<std::string>& _filePaths) {
	/// ----- 引数のファイルパス群をループしてリソースを読み込む ----- ///

	std::string extension;
	for (auto& path : _filePaths) {
		AssetType type = GetAssetTypeFromExtension(FileSystem::FileExtension(path));

		/// 拡張子をチェックして、リソースの種類を決定
		if (type != AssetType::None) {
			Load(path, type);
		}
	}

}

void AssetCollection::UnloadResources(const std::vector<std::string>& _filePaths) {
	/// ----- 引数のファイルパス群をループしてリソースを解放する ----- ///

	std::string extension;
	for (auto& path : _filePaths) {
		/// 拡張子をチェックして、リソースの種類を決定
		AssetType type = GetAssetTypeFromExtension(FileSystem::FileExtension(path));

		/// 拡張子ごとに処理
		switch (type) {
		case AssetType::Texture:
			/// textureの解放
			textureContainer_->Remove(path);
			break;
		case AssetType::Mesh:
			/// meshの解放
			modelContainer_->Remove(path);
			break;
		case AssetType::Audio:
			/// audioの解放
			audioClipContainer_->Remove(path);
			break;
		case AssetType::Material:
			/// materialの解放
			materialContainer_->Remove(path);
			break;
		}

	}

}

void AssetCollection::UnloadAssetByPath(const std::string& _filepath) {
	/// ----- 指定されたファイルパスのアセットを解放する ----- ///

	/// 拡張子をチェックして、リソースの種類を決定
	AssetType type = GetAssetTypeFromExtension(FileSystem::FileExtension(_filepath));

	/// 拡張子ごとに処理
	switch (type) {
	case AssetType::Texture:
		/// textureの解放
		textureContainer_->Remove(_filepath);
		break;
	case AssetType::Mesh:
		/// meshの解放
		modelContainer_->Remove(_filepath);
		break;
	case AssetType::Audio:
		/// audioの解放
		audioClipContainer_->Remove(_filepath);
		break;
	case AssetType::Material:
		/// materialの解放
		materialContainer_->Remove(_filepath);
		break;
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
		if (textureContainer_->GetIndex(_filepath) == -1) {
			auto texture = textureLoader_->Load(_filepath);
			if (texture.has_value()) {
				textureContainer_->Add(_filepath, std::move(texture.value()));
			}
		}
		break;
	case AssetType::Mesh:
		if (modelContainer_->GetIndex(_filepath) == -1) {
			auto model = modelLoader_->Load(_filepath);
			if (model.has_value()) {
				modelContainer_->Add(_filepath, std::move(model.value()));
			}
		}
		break;
	case AssetType::Audio:
		if (audioClipContainer_->GetIndex(_filepath) == -1) {
			auto audioClip = audioClipLoader_->Load(_filepath);
			if (audioClip.has_value()) {
				audioClipContainer_->Add(_filepath, std::move(audioClip.value()));
			}
		}
		break;
	case AssetType::Material:
		if (materialContainer_->GetIndex(_filepath) == -1) {
			auto material = materialLoader_->Load(_filepath);
			if (material.has_value()) {
				materialContainer_->Add(_filepath, std::move(material.value()));
			}
		}
		break;
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


bool AssetCollection::IsAsset(const Guid& _guid) const {
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
	const std::string extension = FileSystem::FileExtension(_filepath);
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
	const std::string extension = FileSystem::FileExtension(_filepath);
	AssetType type = GetAssetTypeFromExtension(extension);


	/// Typeごとにリロード処理を実行
	switch (type) {
	case AssetType::Texture:
		/// Textureのリロード
		if (textureContainer_->GetIndex(_filepath) != -1) {
			Texture* srcTexture = textureContainer_->Get(_filepath);
			auto reloadedTexture = textureLoader_->Reload(_filepath, srcTexture);
			if (reloadedTexture.has_value()) {
				textureContainer_->Add(_filepath, std::move(reloadedTexture.value()));
			}
		}
		break;
	case AssetType::Mesh:
		/// Meshのリロード
		if (modelContainer_->GetIndex(_filepath) != -1) {
			Model* srcModel = modelContainer_->Get(_filepath);
			auto reloadedModel = modelLoader_->Reload(_filepath, srcModel);
			if (reloadedModel.has_value()) {
				modelContainer_->Add(_filepath, std::move(reloadedModel.value()));
			}
		}
		break;
	case AssetType::Audio:
		/// Audioのリロード
		if (audioClipContainer_->GetIndex(_filepath) != -1) {
			AudioClip* srcAudioClip = audioClipContainer_->Get(_filepath);
			auto reloadedAudioClip = audioClipLoader_->Reload(_filepath, srcAudioClip);
			if (reloadedAudioClip.has_value()) {
				audioClipContainer_->Add(_filepath, std::move(reloadedAudioClip.value()));
			}
		}
		break;
	case AssetType::Material:
		/// Materialのリロード
		if (materialContainer_->GetIndex(_filepath) != -1) {
			Material* srcMaterial = materialContainer_->Get(_filepath);
			auto reloadedMaterial = materialLoader_->Reload(_filepath, srcMaterial);
			if (reloadedMaterial.has_value()) {
				materialContainer_->Add(_filepath, std::move(reloadedMaterial.value()));
			}
		}
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
			FileSystem::ReplaceAll(&path, "\\", "/");

			/// 拡張子をチェックして、リソースの種類を決定
			AssetType type = GetAssetTypeFromExtension(FileSystem::FileExtension(path));
			if (type != AssetType::None) {
				resourcePaths.push_back(path);
			}

		}
	}

	return resourcePaths;
}

const Guid& AssetCollection::GetAssetGuidFromPath(const std::string& _filepath) const {
	const std::string extension = FileSystem::FileExtension(_filepath);
	AssetType type = GetAssetTypeFromExtension(extension);

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
