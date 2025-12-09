#include "AssetLoader.h"

/// directX
#include <d3dx12.h>

/// std
#include <fstream>

/// sound api
#include <mfapi.h>
#include <mfobjects.h>
#include <mfidl.h>
#include <mfreadwrite.h>

/// externals/stb_truetype
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype/stb_truetype.h>

/// externals/asssimp
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

/// externals
#include <magic_enum/magic_enum.hpp>

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Asset/Collection/AssetCollection.h"


/// comment
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")


AssetLoader::AssetLoader(DxManager* _dxm, AssetCollection* _collection)
	: pDxManager_(_dxm), pAssetCollection_(_collection) {
}

AssetLoader::~AssetLoader() {
	HRESULT result = MFShutdown();
	Assert(SUCCEEDED(result), "MFShutdown failed");
}



void AssetLoader::Initialize() {
	/// model 
	/// assimpの読み込みフラグ
	assimpLoadFlags_ = aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices;

	/// sound
	HRESULT result = MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
	Assert(SUCCEEDED(result), "MFStartup failed");
}

bool AssetLoader::LoadTextureAuto(const std::string& _filepath) {
	const std::string extension = Mathf::FileExtension(_filepath);
	if (extension == ".dds") {
		DirectX::ScratchImage scratch = LoadScratchImage3D(_filepath);

		const auto& meta = scratch.GetMetadata();
		if (meta.dimension == DirectX::TEX_DIMENSION_TEXTURE3D) {
			return LoadTextureDDS(_filepath);
		}
	}

	/// 3dTexture以外は通常のテクスチャ読み込み
	return LoadTexture(_filepath);
}

bool AssetLoader::LoadTexture([[maybe_unused]] const std::string& _filepath) {
	/// ----- テクスチャの読み込み ----- ///

	/// ファイルが存在するのかチェックする
	if (!std::filesystem::exists(_filepath)) {
		Console::LogError("[Load Failed] [Texture] - File not found: \"" + _filepath + "\"");
		return false;
	}

	/// すでに持っているファイルかチェック
	if (pAssetCollection_->GetTexture(_filepath)) {
		Console::LogWarning("[Load Skipped] [Texture] - Already loaded: \"" + _filepath + "\"");
		return true;
	}


	Texture texture;
	DirectX::ScratchImage       scratchImage = LoadScratchImage(_filepath);
	const DirectX::TexMetadata& metadata = scratchImage.GetMetadata();

	{	/// 読み込む前にテクスチャの情報をログに出力する
		Console::Log("[Texture Load] Path: \"" + _filepath + "\"");
		Console::Log(" - Width: " + std::to_string(metadata.width));
		Console::Log(" - Height: " + std::to_string(metadata.height));
		Console::Log(" - MipLevels: " + std::to_string(metadata.mipLevels));
		Console::Log(" - Format: " + std::string(magic_enum::enum_name(metadata.format)));
		Console::Log(" - Dimension: " + std::string(magic_enum::enum_name(metadata.dimension)));
	}

	texture.dxResource_ = std::move(CreateTextureResource(pDxManager_->GetDxDevice(), metadata));
	if (!texture.dxResource_.Get()) {
		Console::LogError("[Load Failed] [Texture] - Don't Create DxResource: \"" + _filepath + "\"");
		return false;
	}

	texture.dxResource_.Get()->SetName(ConvertString(_filepath).c_str());
	texture.name_ = _filepath;

	DxResource intermediateResource = UploadTextureData(texture.dxResource_.Get(), scratchImage);

	pDxManager_->GetDxCommand()->CommandExecuteAndWait();
	pDxManager_->GetDxCommand()->CommandReset();

	/// metadataを基に srv の設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	if (metadata.IsCubemap()) {
		/// キューブマップの場合
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.MipLevels = UINT_MAX;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	} else {
		/// 2Dテクスチャの場合
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = static_cast<UINT>(metadata.mipLevels);
	}


	/// srv handleの取得
	DxSRVHeap* dxSRVHeap = pDxManager_->GetDxSRVHeap();
	texture.CreateEmptySRVHandle();
	texture.srvHandle_->descriptorIndex = dxSRVHeap->AllocateTexture();
	texture.srvHandle_->cpuHandle = dxSRVHeap->GetCPUDescriptorHandel(texture.srvHandle_->descriptorIndex);
	texture.srvHandle_->gpuHandle = dxSRVHeap->GetGPUDescriptorHandel(texture.srvHandle_->descriptorIndex);

	/// srvの生成
	DxDevice* dxDevice = pDxManager_->GetDxDevice();
	dxDevice->GetDevice()->CreateShaderResourceView(texture.dxResource_.Get(), &srvDesc, texture.srvHandle_->cpuHandle);


	/// texture size
	Vector2 textureSize = { static_cast<float>(metadata.width), static_cast<float>(metadata.height) };
	texture.textureSize_ = textureSize;

	/// テクスチャフォーマット
	texture.srvFormat_ = metadata.format;


	/// ログにテクスチャの情報を書き出す
	Console::Log("[Success Texture Info] Path: \"" + _filepath + "\"");
	Console::Log(" - Width: " + std::to_string(metadata.width));
	Console::Log(" - Height: " + std::to_string(metadata.height));
	Console::Log(" - MipLevels: " + std::to_string(metadata.mipLevels));
	Console::Log(" - Format: " + std::string(magic_enum::enum_name(metadata.format)));
	Console::Log(" - DescriptorIndex: " + std::to_string(texture.srvHandle_->descriptorIndex));
	Console::Log(" - Dimension: Texture2D");


	pAssetCollection_->AddAsset<Texture>(_filepath, std::move(texture));

	return true;
}


bool AssetLoader::LoadTextureDDS(const std::string& _filepath) {
	/// ----- DDSファイルの読み込み ----- ///

	/// ファイルが存在するのかチェックする
	if (!std::filesystem::exists(_filepath)) {
		Console::LogError("[Load Failed] [Texture DDS] - File not found: \"" + _filepath + "\"");
		return false;
	}

	/// すでに持っているファイルかチェック
	if (pAssetCollection_->GetTexture(_filepath)) {
		Console::LogWarning("[Load Skipped] [Texture DDS] - Already loaded: \"" + _filepath + "\"");
		return true;
	}


	Texture texture;


	/// スクラッチイメージを読み込み、使用可能かチェック
	DirectX::ScratchImage scratchImage = LoadScratchImage3D(_filepath);
	if (scratchImage.GetImageCount() == 0) {
		Console::LogError("[Load Failed] [Texture DDS] - Failed to load DDS file: \"" + _filepath + "\"");
		return false;
	}


	/// metadataは3Dテクスチャでないといけない
	const DirectX::TexMetadata& metadata = scratchImage.GetMetadata();
	if (metadata.dimension != DirectX::TEX_DIMENSION_TEXTURE3D) {
		Console::LogError("[Load Failed] [Texture DDS] - Not a 3D texture: \"" + _filepath + "\"");
		return false;
	}


	texture.dxResource_ = std::move(CreateTexture3DResource(pDxManager_->GetDxDevice(), metadata));
	if (!texture.dxResource_.Get()) {
		Console::LogError("[Load Failed] [Texture DDS] - Don't Create DxResource: \"" + _filepath + "\"");
		return false;
	}

	texture.dxResource_.Get()->SetName(ConvertString(_filepath).c_str());
	texture.dxResource_.SetCurrentState(D3D12_RESOURCE_STATE_GENERIC_READ);
	texture.name_ = _filepath;

	DxResource intermediateResource = UploadTextureData(texture.dxResource_.Get(), scratchImage);

	pDxManager_->GetDxCommand()->CommandExecuteAndWait();
	pDxManager_->GetDxCommand()->CommandReset();

	/// metadataを基に srv の設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture3D.MipLevels = static_cast<UINT16>(metadata.mipLevels);


	/// srv handleの取得
	DxSRVHeap* dxSRVHeap = pDxManager_->GetDxSRVHeap();
	texture.CreateEmptySRVHandle();
	texture.srvHandle_->descriptorIndex = dxSRVHeap->AllocateTexture();
	texture.srvHandle_->cpuHandle = dxSRVHeap->GetCPUDescriptorHandel(texture.srvHandle_->descriptorIndex);
	texture.srvHandle_->gpuHandle = dxSRVHeap->GetGPUDescriptorHandel(texture.srvHandle_->descriptorIndex);

	/// srvの生成
	DxDevice* dxDevice = pDxManager_->GetDxDevice();
	dxDevice->GetDevice()->CreateShaderResourceView(texture.dxResource_.Get(), &srvDesc, texture.srvHandle_->cpuHandle);


	/// テクスチャサイズ
	Vector2 textureSize = { static_cast<float>(metadata.width), static_cast<float>(metadata.height) };
	texture.textureSize_ = textureSize;
	texture.depth_ = static_cast<uint32_t>(metadata.depth);

	/// テクスチャフォーマット
	texture.srvFormat_ = metadata.format;


	/// ログにテクスチャの情報を書き出す
	Console::Log("[Texture DDS Info] Path: \"" + _filepath + "\"");
	Console::Log(" - Width: " + std::to_string(metadata.width));
	Console::Log(" - Height: " + std::to_string(metadata.height));
	Console::Log(" - Depth: " + std::to_string(metadata.depth));
	Console::Log(" - MipLevels: " + std::to_string(metadata.mipLevels));
	Console::Log(" - Format: " + std::string(magic_enum::enum_name(metadata.format)));
	Console::Log(" - DescriptorIndex: " + std::to_string(texture.srvHandle_->descriptorIndex));
	Console::Log(" - Dimension: Texture3D");
	


	pAssetCollection_->AddAsset<Texture>(_filepath, std::move(texture));

	return true;
}


bool AssetLoader::ReloadTextureAuto(const std::string& _filepath) {
	/// ファイルの拡張子を確認して、DDSなら3Dテクスチャとしてリロード

	const std::string extension = Mathf::FileExtension(_filepath);
	if (extension == ".dds") {
		DirectX::ScratchImage scratch = LoadScratchImage3D(_filepath);
		const auto& meta = scratch.GetMetadata();
		if (meta.dimension == DirectX::TEX_DIMENSION_TEXTURE3D) {
			return ReloadTextureDDS(_filepath);
		}
	}

	/// 3Dテクスチャ以外は通常のテクスチャリロード
	return ReloadTexture(_filepath);
}

bool AssetLoader::ReloadTexture(const std::string& _filepath) {
	/// ----- テクスチャのリロード ----- ///

	/// ファイルが存在するのかチェックする
	if (!std::filesystem::exists(_filepath)) {
		Console::LogError("[Reload Failed] [Texture] - File not found: \"" + _filepath + "\"");
		return false;
	}

	Texture* existingTexture = pAssetCollection_->GetTexture(_filepath);
	if (!existingTexture) {
		Console::LogError("[Reload Failed] [Texture] - Asset not found: \"" + _filepath + "\"");
		return false;
	}

	DirectX::ScratchImage       scratchImage = LoadScratchImage(_filepath);
	const DirectX::TexMetadata& metadata = scratchImage.GetMetadata();

	/// metadataを確認して、サイズが0などの場合であればリロード失敗
	if (metadata.width == 0 || metadata.height == 0) {
		Console::LogError("[Reload Failed] [Texture] - Invalid texture metadata: \"" + _filepath + "\"");
		return false;
	}


	existingTexture->dxResource_ = std::move(CreateTextureResource(pDxManager_->GetDxDevice(), metadata));
	DxResource intermediateResource = UploadTextureData(existingTexture->dxResource_.Get(), scratchImage);

	pDxManager_->GetDxCommand()->CommandExecuteAndWait();
	pDxManager_->GetDxCommand()->CommandReset();


	/// SRVの上書き
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = static_cast<UINT>(metadata.mipLevels);

	auto dxDevice = pDxManager_->GetDxDevice();
	dxDevice->GetDevice()->CreateShaderResourceView(existingTexture->dxResource_.Get(), &srvDesc, existingTexture->srvHandle_->cpuHandle);

	/// texture size
	Vector2 textureSize = { static_cast<float>(metadata.width), static_cast<float>(metadata.height) };
	existingTexture->textureSize_ = textureSize;


	Console::Log("[Reload] [Texture] - path:\"" + _filepath + "\"");
	return true;
}

bool AssetLoader::ReloadTextureDDS(const std::string& _filepath) {
	/// ----- 3DテクスチャのDDSファイルのリロード ----- ///

	/// ファイルが存在するのかチェックする
	if (!std::filesystem::exists(_filepath)) {
		Console::LogError("[Reload Failed] [Texture DDS] - File not found: \"" + _filepath + "\"");
		return false;
	}

	/// まだ読み込んでいないファイルであればReloadの対象ではない、通常の読み込みを促す
	Texture* existingTexture = pAssetCollection_->GetTexture(_filepath);
	if (!existingTexture) {
		Console::LogError("[Reload Failed] [Texture DDS] - Asset not found: \"" + _filepath + "\"");
		return false;
	}


	/// スクラッチイメージを読み込み、使用可能かチェック
	DirectX::ScratchImage scratchImage = LoadScratchImage3D(_filepath);
	const DirectX::TexMetadata& metadata = scratchImage.GetMetadata();

	/// 使用可能チェック
	if (metadata.width == 0 || metadata.height == 0 || metadata.depth == 0) {
		Console::LogError("[Reload Failed] [Texture] - Invalid texture metadata: \"" + _filepath + "\"");
		return false;
	}

	/// 読み直し
	existingTexture->dxResource_ = std::move(CreateTextureResource(pDxManager_->GetDxDevice(), metadata));
	DxResource intermediateResource = UploadTextureData(existingTexture->dxResource_.Get(), scratchImage);

	pDxManager_->GetDxCommand()->CommandExecuteAndWait();
	pDxManager_->GetDxCommand()->CommandReset();

	/// SRVの上書き
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture3D.MipLevels = static_cast<UINT16>(metadata.mipLevels);

	auto dxDevice = pDxManager_->GetDxDevice();
	dxDevice->GetDevice()->CreateShaderResourceView(existingTexture->dxResource_.Get(), &srvDesc, existingTexture->srvHandle_->cpuHandle);

	/// テクスチャのデータを更新
	Vector2 textureSize = { static_cast<float>(metadata.width), static_cast<float>(metadata.height) };
	existingTexture->textureSize_ = textureSize;
	existingTexture->depth_ = static_cast<UINT>(metadata.depth);
	existingTexture->srvFormat_ = metadata.format;


	/// ログにテクスチャの情報を書き出す
	Console::Log("[Reload] [Texture DDS] - path:\"" + _filepath + "\"");
	Console::Log(" - Width: " + std::to_string(metadata.width));
	Console::Log(" - Height: " + std::to_string(metadata.height));
	Console::Log(" - Depth: " + std::to_string(metadata.depth));
	Console::Log(" - MipLevels: " + std::to_string(metadata.mipLevels));
	Console::Log(" - Format: " + std::string(magic_enum::enum_name(metadata.format)));

	return true;
}


bool AssetLoader::LoadModelObj(const std::string& _filepath) {
	/// ----- モデルの読み込み ----- ///

	/// ファイルが存在するのかチェックする
	if (!std::filesystem::exists(_filepath)) {
		Console::LogError("[Load Failed] [Model] - File not found: \"" + _filepath + "\"");
		return false;
	}

	/// ファイルの拡張子を取得
	std::string fileExtension = Mathf::FileExtension(_filepath);
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(_filepath, assimpLoadFlags_);

	if (!scene) {
		return false; ///< 読み込み失敗
	}

	Model model;
	model.SetPath(_filepath);

	/// mesh 解析
	for (uint32_t meshIndex = 0u; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		Assert(mesh->HasNormals(), "mesh has no normals");
		Assert(mesh->HasTextureCoords(0), "mesh has no texture coords");

		/// sceneのデータを使ってMeshを作成する
		std::vector<Mesh::VertexData> vertices;
		std::vector<uint32_t>         indices;

		vertices.reserve(mesh->mNumVertices);
		indices.reserve(mesh->mNumFaces * 3);


		/// vertex 解析
		for (uint32_t i = 0; i < mesh->mNumVertices; ++i) {
			Mesh::VertexData&& vertex = {
				Vector4(-mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.0f),
				Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y),
				Vector3(-mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z)
			};

			vertices.push_back(vertex);
		}


		/// index 解析
		for (uint32_t i = 0; i < mesh->mNumFaces; ++i) {
			aiFace face = mesh->mFaces[i];
			Assert(face.mNumIndices == 3, "not triangles");

			for (uint32_t j = 0; j < face.mNumIndices; ++j) {
				indices.push_back(face.mIndices[j]);
			}
		}


		/// joint 解析
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {

			/// 格納領域の作成
			aiBone* bone = mesh->mBones[boneIndex];
			std::string      jointName = bone->mName.C_Str();
			JointWeightData& jointWeightData = model.GetJointWeightData()[jointName];

			/// mat bind pose inverseの計算
			aiMatrix4x4  matBindPoseAssimp = bone->mOffsetMatrix.Inverse();
			aiVector3D   position;
			aiQuaternion rotate;
			aiVector3D   scale;

			matBindPoseAssimp.Decompose(scale, rotate, position);
			Matrix4x4 matBindPose =
				Matrix4x4::MakeScale({ scale.x, scale.y, scale.z })
				* Matrix4x4::MakeRotate(Quaternion::Normalize({ rotate.x, -rotate.y, -rotate.z, rotate.w }))
				* Matrix4x4::MakeTranslate({ -position.x, position.y, position.z });

			jointWeightData.matBindPoseInverse = matBindPose.Inverse();


			/// weight情報を取り出す
			for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
				jointWeightData.vertexWeights.push_back(
					{ bone->mWeights[weightIndex].mWeight, bone->mWeights[weightIndex].mVertexId }
				);
			}

		}

		if (fileExtension == ".gltf") {
			/// nodeの解析
			model.SetRootNode(ReadNode(scene->mRootNode));
			LoadAnimation(&model, _filepath);
		}

		/// mesh dataを作成
		std::unique_ptr<Mesh> meshData = std::make_unique<Mesh>();
		meshData->SetVertices(vertices);
		meshData->SetIndices(indices);

		/// bufferの作成
		meshData->CreateBuffer(pDxManager_->GetDxDevice());

		model.AddMesh(std::move(meshData));
	}

	Console::Log("[Load] [Model] - path:\"" + _filepath + "\"");
	pAssetCollection_->AddAsset<Model>(_filepath, std::move(model));

	return true;
}

Node AssetLoader::ReadNode(aiNode* _node) {
	/// ----- nodeの読み込み ----- ///

	Node result;

	aiVector3D   position;
	aiQuaternion rotate;
	aiVector3D   scale;

	_node->mTransformation.Decompose(scale, rotate, position);

	result.transform.scale = { scale.x, scale.y, scale.z };
	result.transform.rotate = { rotate.x, -rotate.y, -rotate.z, rotate.w };
	result.transform.position = { -position.x, position.y, position.z };
	result.transform.Update();

	/// nodeから必要な値をゲット
	result.name = _node->mName.C_Str();
	result.children.resize(_node->mNumChildren);

	/// childrenの解析
	for (size_t childIndex = 0; childIndex < _node->mNumChildren; ++childIndex) {
		result.children[childIndex] = ReadNode(_node->mChildren[childIndex]);
	}

	return result;
}

void AssetLoader::LoadAnimation(Model* _model, const std::string& _filepath) {
	/// ----- アニメーションの読み込み ----- ///

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(_filepath.c_str(), 0);

	///!< アニメーションが存在しない場合は何もしない
	if (scene->mAnimations == 0) {
		Console::Log("[warring] type:Animation, path:\"" + _filepath + "\"");
		return; ///< アニメーションが存在しない場合は何もしない
	}

	/// 解析
	aiAnimation* animationAssimp = scene->mAnimations[0];

	float duration = _model->GetAnimationDuration();
	std::unordered_map<std::string, NodeAnimation>& nodeAnimationMap = _model->GetNodeAnimationMap();

	duration = static_cast<float>(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);
	_model->SetAnimationDuration(duration);

	/// node animationの読み込み
	for (uint32_t channelIndex = 0u; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {

		/// node animationの解析用データを
		aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
		NodeAnimation& nodeAnimation = nodeAnimationMap[nodeAnimationAssimp->mNodeName.C_Str()];

		/// ---------------------------------------------------
		/// translateの解析
		/// ---------------------------------------------------
		for (uint32_t keyIndex = 0u; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {

			/// keyの値を得る
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
			KeyFrameVector3 keyframe{
				.time = static_cast<float>(keyAssimp.mTime / animationAssimp->mTicksPerSecond),
				.value = { -keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z }
			};

			nodeAnimation.translate.push_back(keyframe);
		}


		/// ---------------------------------------------------
		/// rotateの解析
		/// ---------------------------------------------------
		for (uint32_t keyIndex = 0u; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {

			/// keyの値を得る
			aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
			KeyFrameQuaternion keyframe{
				.time = static_cast<float>(keyAssimp.mTime / animationAssimp->mTicksPerSecond),
				.value = { keyAssimp.mValue.x, -keyAssimp.mValue.y, -keyAssimp.mValue.z, keyAssimp.mValue.w }
			};

			nodeAnimation.rotate.push_back(keyframe);
		}


		/// ---------------------------------------------------
		/// scaleの解析
		/// ---------------------------------------------------
		for (uint32_t keyIndex = 0u; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {

			/// keyの値を得る
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
			KeyFrameVector3 keyframe{
				.time = static_cast<float>(keyAssimp.mTime / animationAssimp->mTicksPerSecond),
				.value = { keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z }
			};

			nodeAnimation.scale.push_back(keyframe);
		}

	}
}

bool AssetLoader::LoadAudioClip(const std::string& _filepath) {
	/// ----- オーディオクリップの読み込み ----- ///

	/// ファイルが存在するのかチェックする
	if (!std::filesystem::exists(_filepath)) {
		Console::LogError("[Load Failed] [AudioClip] - File not found: \"" + _filepath + "\"");
		return false;
	}

	/// wstringに変換
	std::wstring filePathW = ConvertString(_filepath);
	HRESULT result;

	/// SourceReaderの作成
	ComPtr<IMFSourceReader> sourceReader;
	result = MFCreateSourceReaderFromURL(filePathW.c_str(), nullptr, &sourceReader);
	if (!SUCCEEDED(result)) {
		Console::LogError("[Load Failed] [AudioClip] - MFCreateSourceReaderFromURL failed: \"" + _filepath + "\"");
		return false;
	}

	/// PCM形式にフォーマットを指定する
	ComPtr<IMFMediaType> audioType;
	MFCreateMediaType(&audioType);
	audioType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	audioType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	result = sourceReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, audioType.Get());
	if (!SUCCEEDED(result)) {
		Console::LogError("[Load Failed] [AudioClip] - SetCurrentMediaType failed: \"" + _filepath + "\"");
		return false;
	}

	/// メディアタイプの取得
	ComPtr<IMFMediaType> pOutType;
	sourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pOutType);

	/// waveフォーマットの取得
	WAVEFORMATEX* waveFormat = nullptr;
	MFCreateWaveFormatExFromMFMediaType(pOutType.Get(), &waveFormat, nullptr);


	/// コンテナに格納する用のデータを作成する
	AudioStructs::SoundData soundData{};
	soundData.wfex = *waveFormat;

	/// 作成したwaveフォーマットを解放
	CoTaskMemFree(waveFormat);

	/// PCMデータのバッファを構築
	while (true) {
		ComPtr<IMFSample> pSample;
		DWORD streamIndex = 0;
		DWORD flags = 0;
		LONGLONG llTimeStamp = 0;

		/// サンプルを読み込む
		result = sourceReader->ReadSample(
			(DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM,
			0,
			&streamIndex, &flags,
			&llTimeStamp, &pSample
		);

		/// ストリームが終了したら抜ける
		if (flags & MF_SOURCE_READERF_ENDOFSTREAM) {
			break;
		}

		if (pSample) {
			ComPtr<IMFMediaBuffer> pBuffer;
			/// サンプルに含まれるサウンドデータのバッファを一繋ぎにして取得
			pSample->ConvertToContiguousBuffer(&pBuffer);

			BYTE* pData = nullptr;
			DWORD maxLength = 0;
			DWORD currentLength = 0;
			/// バッファ読み込み用にロック
			pBuffer->Lock(&pData, &maxLength, &currentLength);
			/// バッファの末尾にデータを追加
			soundData.buffer.insert(soundData.buffer.end(), pData, pData + currentLength);
			pBuffer->Unlock();

		}

	}


	AudioClip audioClip;
	audioClip.soundData_ = std::move(soundData);
	pAssetCollection_->AddAsset<AudioClip>(_filepath, std::move(audioClip));

	Console::Log("[Load] [AudioClip] - path:\"" + _filepath + "\"");
	return true;
}

bool AssetLoader::LoadMaterial(const std::string& _filepath) {

	/// ファイル拡張子を確認 Materialファイル以外は処理しない
	const std::string ext = Mathf::FileExtension(_filepath);
	if (ext != ".mat") {
		return false;
	}

	/// ファイルを開く
	std::ifstream ifs(_filepath);
	if (!ifs) {
		Console::LogError("[Load Failed] [Material] - File not found: \"" + _filepath + "\"");
		return false;
	}


	/// 読み込んだMaterialを格納するオブジェクト
	Material material;

	/// ----------------------------------------------
	/// ファイルの読み込み
	/// ----------------------------------------------

	std::string line;
	while (std::getline(ifs, line)) {
		/// ----- 各文字列ごとに対応した処理を行う ----- ///

		if (Mathf::StartsWith(line, "guid: ")) {
			/// ファイルの文字列をGuidに変換して格納
			std::string guidStr = line.substr(6);
			material.guid = Guid::FromString(guidStr);
		}

	}


	/// コンソールにログを出力
	Console::Log("[Load] [Material] - path:\"" + _filepath + "\"");

	/// AssetCollectionにMaterialを追加
	pAssetCollection_->AddAsset<Material>(_filepath, std::move(material));

	return true;
}

void AssetLoader::LoadFont(const std::string& _filepath) {

	/// ファイルを開く
	std::ifstream ifs(_filepath, std::ios::binary | std::ios::ate);
	if (!ifs) {
		Assert(false, ("failed to open file: " + _filepath).c_str());
		return;
	}

	std::streamsize size = ifs.tellg();
	ifs.seekg(0, std::ios::beg);

	/// .ttfのバッファ
	std::vector<unsigned char> buffer(size);
	if (!ifs.read((char*)buffer.data(), size)) {
		Assert(false, ("failed to read file: " + _filepath).c_str());
		return;
	}

	/// フォント情報を初期化
	stbtt_fontinfo fontInfo;
	if (!stbtt_InitFont(&fontInfo, buffer.data(), 0)) {
		Assert(false, ("failed to initialize font: " + _filepath).c_str());
		return;
	}

}

DirectX::ScratchImage AssetLoader::LoadScratchImage(const std::string& _filePath) {

	/// テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring          filePathW = ConvertString(_filePath);
	if (_filePath.ends_with(".dds")) {
		DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	} else {
		DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	}

	/// mipMapの作成
	DirectX::ScratchImage mipImages{};

	/// 圧縮されていたらそのままimageを使うようにする
	if (DirectX::IsCompressed(image.GetMetadata().format)) {
		mipImages = std::move(image);
	} else {
		DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	}

	return mipImages;
}

DirectX::ScratchImage AssetLoader::LoadScratchImage3D(const std::string& _filePath) {
	// DDS ファイル専用
	if (!_filePath.ends_with(".dds")) {
		Console::LogError("LoadScratchImage3D: Only DDS files are supported for Texture3D.");
		return DirectX::ScratchImage{};
	}

	std::wstring filePathW = ConvertString(_filePath);
	DirectX::ScratchImage image;

	HRESULT hr = DirectX::LoadFromDDSFile(
		filePathW.c_str(),
		DirectX::DDS_FLAGS_NONE,
		nullptr,
		image
	);
	if (FAILED(hr)) {
		Console::LogError("[Load Failed] Texture3D DDS: \"" + _filePath + "\"");
		return DirectX::ScratchImage{};
	}

	// Texture3D かどうかチェック
	const DirectX::TexMetadata& meta = image.GetMetadata();
	if (meta.dimension != DirectX::TEX_DIMENSION_TEXTURE3D) {
		Console::LogError("[Load Failed] File is not a Texture3D DDS: \"" + _filePath + "\"");
		return DirectX::ScratchImage{};
	}

	// mipMap生成（圧縮済みならスキップ）
	DirectX::ScratchImage mipImages;
	if (DirectX::IsCompressed(meta.format)) {
		mipImages = std::move(image);
	} else {
		hr = DirectX::GenerateMipMaps(
			image.GetImages(),
			image.GetImageCount(),
			meta,
			DirectX::TEX_FILTER_DEFAULT, // sRGB対応の場合は TEX_FILTER_SRGB に変更
			0,
			mipImages
		);
		if (FAILED(hr)) {
			Console::LogWarning("[GenerateMipMaps Failed] Using original image: \"" + _filePath + "\"");
			mipImages = std::move(image);
		}
	}

	return mipImages;
}


DxResource AssetLoader::CreateTextureResource(DxDevice* _dxDevice, const DirectX::TexMetadata& _metadata) {

	/// --------------------------------------
	/// ↓ metadataを基にResourceの設定
	/// --------------------------------------

	D3D12_RESOURCE_DESC desc{};

	/// テクスチャの幅
	desc.Width = UINT(_metadata.width);
	desc.Height = UINT(_metadata.height);

	/// テクスチャの深さ
	desc.MipLevels = UINT16(_metadata.mipLevels);
	desc.DepthOrArraySize = UINT16(_metadata.arraySize);                   /// 奥行き or 配列Textureの配列数
	desc.Format = _metadata.format;                              /// TextureのFormat
	desc.SampleDesc.Count = 1;                                             /// サンプリングカウント; 1固定
	desc.Dimension = D3D12_RESOURCE_DIMENSION(_metadata.dimension); /// Textureの次元数


	/// --------------------------------------
	/// ↓ 利用するHeapの設定
	/// --------------------------------------

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;	//- 細かい設定を行う


	/// --------------------------------------
	/// ↓ Resourceを生成
	/// --------------------------------------

	DxResource dxResource;
	dxResource.CreateCommittedResource(
		_dxDevice,
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr
	);

	return dxResource;
}

DxResource AssetLoader::CreateTexture3DResource(DxDevice* _dxDevice, const DirectX::TexMetadata& _metadata) {
	if (_metadata.dimension != DirectX::TEX_DIMENSION_TEXTURE3D) {
		Console::LogError("[CreateTexture3DResource] Metadata is not Texture3D.");
		return DxResource{};
	}

	D3D12_RESOURCE_DESC desc{};
	desc.Width = static_cast<UINT>(_metadata.width);
	desc.Height = static_cast<UINT>(_metadata.height);
	desc.DepthOrArraySize = static_cast<UINT16>(_metadata.depth); // Texture3Dの奥行き
	desc.MipLevels = static_cast<UINT16>(_metadata.mipLevels);
	desc.Format = _metadata.format;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	DxResource dxResource;
	dxResource.CreateCommittedResource(
		_dxDevice,
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr
	);

	return dxResource;
}

DxResource AssetLoader::UploadTextureData(ID3D12Resource* _texture, const DirectX::ScratchImage& _mipScratchImage) {
	DxDevice* dxDevice = pDxManager_->GetDxDevice();
	DxCommand* dxCommand = pDxManager_->GetDxCommand();

	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(dxDevice->GetDevice(), _mipScratchImage.GetImages(), _mipScratchImage.GetImageCount(), _mipScratchImage.GetMetadata(), subresources);
	uint64_t   intermediateSize = GetRequiredIntermediateSize(_texture, 0, static_cast<UINT>(subresources.size()));
	DxResource intermediateDxResource;
	intermediateDxResource.CreateResource(dxDevice, intermediateSize);

	UpdateSubresources(dxCommand->GetCommandList(), _texture, intermediateDxResource.Get(), 0, 0, static_cast<UINT>(subresources.size()), subresources.data());
	CreateBarrier(_texture, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ, dxCommand);

	return intermediateDxResource;
}

