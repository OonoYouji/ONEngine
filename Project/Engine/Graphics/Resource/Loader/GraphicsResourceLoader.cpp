#include "GraphicsResourceLoader.h"

/// directX
#include <d3dx12.h>

/// externals
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/Utility/Tools/Assert.h"
#include "Engine/Core/Utility/Tools/Log.h"
#include "../GraphicsResourceCollection.h"


GraphicsResourceLoader::GraphicsResourceLoader(DxManager* _dxManager, GraphicsResourceCollection* _collection)
	: dxManager_(_dxManager), resourceCollection_(_collection) {}

GraphicsResourceLoader::~GraphicsResourceLoader() {}



void GraphicsResourceLoader::Initialize() {

	/// assimpの読み込みフラグ
	assimpLoadFlags_ = aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices;

}

void GraphicsResourceLoader::LoadTexture([[maybe_unused]] const std::string& _filePath) {

	std::unique_ptr<Texture>    texture = std::make_unique<Texture>();
	DirectX::ScratchImage       scratchImage = LoadScratchImage(_filePath);
	const DirectX::TexMetadata& metadata = scratchImage.GetMetadata();

	texture->dxResource_ = std::move(CreateTextureResource(dxManager_->GetDxDevice(), metadata));
	DxResource intermediateResource = UploadTextureData(texture->dxResource_.Get(), scratchImage);

	dxManager_->GetDxCommand()->CommandExecute();
	dxManager_->GetDxCommand()->CommandReset();

	/// metadataを基に srv の設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	if (metadata.IsCubemap()) {
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.MipLevels = UINT_MAX;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;

	} else {
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = static_cast<UINT>(metadata.mipLevels);

	}



	/// srv handleの取得
	DxSRVHeap* dxSRVHeap = dxManager_->GetDxSRVHeap();

	texture->CreateEmptySRVHandle();
	texture->srvHandle_->descriptorIndex = dxSRVHeap->AllocateTexture();
	texture->srvHandle_->cpuHandle = dxSRVHeap->GetCPUDescriptorHandel(texture->srvHandle_->descriptorIndex);
	texture->srvHandle_->gpuHandle = dxSRVHeap->GetGPUDescriptorHandel(texture->srvHandle_->descriptorIndex);

	/// srvの生成
	DxDevice* dxDevice = dxManager_->GetDxDevice();
	dxDevice->GetDevice()->CreateShaderResourceView(texture->dxResource_.Get(), &srvDesc, texture->srvHandle_->cpuHandle);

	Console::Log("[Load Resource] type:Texture, path:\"" + _filePath + "\"");
	resourceCollection_->AddTexture(_filePath, std::move(texture));
}

void GraphicsResourceLoader::LoadModelObj(const std::string& _filePath) {

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(_filePath, assimpLoadFlags_);

	if (!scene) {
		return; ///< 読み込み失敗
	}

	std::unique_ptr<Model> model = std::make_unique<Model>();
	model->SetPath(_filePath);

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
				Vec4(-mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.0f),
				Vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y),
				Vec3(-mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z)
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


		/// mesh dataを作成
		std::unique_ptr<Mesh> meshData = std::make_unique<Mesh>();
		meshData->SetVertices(vertices);
		meshData->SetIndices(indices);

		/// bufferの作成
		meshData->CreateBuffer(dxManager_->GetDxDevice());

		model->AddMesh(std::move(meshData));
	}

	Console::Log("[Load Resource] type:Model, path:\"" + _filePath + "\"");
	resourceCollection_->AddModel(_filePath, std::move(model));

}

DirectX::ScratchImage GraphicsResourceLoader::LoadScratchImage(const std::string& _filePath) {

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

DxResource GraphicsResourceLoader::CreateTextureResource(DxDevice* _dxDevice, const DirectX::TexMetadata& _metadata) {

	/// --------------------------------------
	/// ↓ metadataを基にResourceの設定
	/// --------------------------------------

	D3D12_RESOURCE_DESC desc{};
	desc.Width = UINT(_metadata.width);                         /// textureの幅
	desc.Height = UINT(_metadata.height);                        /// textureの高さ
	desc.MipLevels = UINT16(_metadata.mipLevels);                   /// mipmapの数
	desc.DepthOrArraySize = UINT16(_metadata.arraySize);                   /// 奥行き or 配列Textureの配列数
	desc.Format = _metadata.format;                              /// TextureのFormat
	desc.SampleDesc.Count = 1;                                             /// サンプリングカウント; 1固定
	desc.Dimension = D3D12_RESOURCE_DIMENSION(_metadata.dimension); /// Textureの次元数
	//desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;


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

DxResource GraphicsResourceLoader::UploadTextureData(ID3D12Resource* _texture, const DirectX::ScratchImage& _mipScratchImage) {
	DxDevice* dxDevice = dxManager_->GetDxDevice();
	DxCommand* dxCommand = dxManager_->GetDxCommand();

	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(dxDevice->GetDevice(), _mipScratchImage.GetImages(), _mipScratchImage.GetImageCount(), _mipScratchImage.GetMetadata(), subresources);
	uint64_t   intermediateSize = GetRequiredIntermediateSize(_texture, 0, static_cast<UINT>(subresources.size()));
	DxResource intermediateDxResource;
	intermediateDxResource.CreateResource(dxDevice, intermediateSize);

	UpdateSubresources(dxCommand->GetCommandList(), _texture, intermediateDxResource.Get(), 0, 0, static_cast<UINT>(subresources.size()), subresources.data());
	CreateBarrier(_texture, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ, dxCommand);

	return intermediateDxResource;
}

