#include "TextureLoader.h"

/// externals
#include <magic_enum/magic_enum.hpp>

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/Utility/Utility.h"

using namespace ONEngine;

AssetLoaderT<Texture>::AssetLoaderT(DxManager* _dxm, AssetCollection* _ac)
	: pDxManager_(_dxm), pAssetCollection_(_ac) {
}

Texture AssetLoaderT<Texture>::Load(const std::string& _filepath) {
	/// 3Dテクスチャか2Dテクスチャかを判別して読み込みを行う
	/// 3Dテクスチャか判別し、3Dなら3Dテクスチャとして読み込む
	const std::string extension = FileSystem::FileExtension(_filepath);
	if (extension == ".dds") {
		DirectX::ScratchImage scratch = LoadScratchImage3D(_filepath);
		const auto& meta = scratch.GetMetadata();
		if (meta.dimension == DirectX::TEX_DIMENSION_TEXTURE3D) {
			return Load3DTexture(_filepath);
		}
	}

	/// 2Dテクスチャとして読み込む(DDSファイルでも2Dテクスチャならここで読む)
	return Load2DTexture(_filepath);
}

Texture AssetLoaderT<Texture>::Reload(const std::string& _filepath, Texture* _src) {
	const std::string extension = FileSystem::FileExtension(_filepath);
	if (extension == ".dds") {
		DirectX::ScratchImage scratch = LoadScratchImage3D(_filepath);
		const auto& meta = scratch.GetMetadata();
		if (meta.dimension == DirectX::TEX_DIMENSION_TEXTURE3D) {
			return Reload3DTexture(_filepath, _src);
		}
	}

	/// 3Dテクスチャ以外は通常のテクスチャリロード
	return Reload2DTexture(_filepath, _src);
}


Texture AssetLoaderT<Texture>::Load2DTexture(const std::string& _filepath) {
	/// ----- テクスチャの読み込み ----- ///

	Texture texture;
	DirectX::ScratchImage       scratchImage = LoadScratchImage2D(_filepath);
	const DirectX::TexMetadata& metadata = scratchImage.GetMetadata();

	{	/// 読み込む前にテクスチャの情報をログに出力する
		Console::Log("[Load Texture Info] Path: \"" + _filepath + "\"");
		Console::Log(" - Width: " + std::to_string(metadata.width));
		Console::Log(" - Height: " + std::to_string(metadata.height));
		Console::Log(" - MipLevels: " + std::to_string(metadata.mipLevels));
		Console::Log(" - Format: " + std::string(magic_enum::enum_name(metadata.format)));
		Console::Log(" - Dimension: " + std::string(magic_enum::enum_name(metadata.dimension)));
	}

	texture.dxResource_ = std::move(CreateTextureResource2D(pDxManager_->GetDxDevice(), metadata));
	if (!texture.dxResource_.Get()) {
		Console::LogError("[Load Failed] [Texture] - Don't Create DxResource: \"" + _filepath + "\"");
		return {};
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


	{	/// ログに完了したテクスチャの情報を書き出す
		Console::Log("[Success Texture Info] Path: \"" + _filepath + "\"");
		Console::Log(" - DescriptorIndex: " + std::to_string(texture.srvHandle_->descriptorIndex));
	}

	return std::move(texture);
}

Texture AssetLoaderT<Texture>::Load3DTexture(const std::string& _filepath) {
	/// ----- DDSファイルの読み込み ----- ///
	Texture texture;

	/// スクラッチイメージを読み込み、使用可能かチェック
	DirectX::ScratchImage scratchImage = LoadScratchImage3D(_filepath);
	if (scratchImage.GetImageCount() == 0) {
		Console::LogError("[Load Failed] [Texture DDS] - Failed to load DDS file: \"" + _filepath + "\"");
		return {};
	}


	/// metadataは3Dテクスチャでないといけない
	const DirectX::TexMetadata& metadata = scratchImage.GetMetadata();
	if (metadata.dimension != DirectX::TEX_DIMENSION_TEXTURE3D) {
		Console::LogError("[Load Failed] [Texture DDS] - Not a 3D texture: \"" + _filepath + "\"");
		return {};
	}


	texture.dxResource_ = std::move(CreateTextureResource3D(pDxManager_->GetDxDevice(), metadata));
	if (!texture.dxResource_.Get()) {
		Console::LogError("[Load Failed] [Texture DDS] - Don't Create DxResource: \"" + _filepath + "\"");
		return {};
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
	Console::Log("");

	return std::move(texture);
}


Texture AssetLoaderT<Texture>::Reload2DTexture(const std::string& _filepath, Texture* _src) {
	/// ----- テクスチャの読み込み ----- ///
	Texture texture = *_src; // 元のテクスチャをコピー
	DirectX::ScratchImage       scratchImage = LoadScratchImage2D(_filepath);
	const DirectX::TexMetadata& metadata = scratchImage.GetMetadata();
	/// texture resource の更新
	DxResource newResource = CreateTextureResource2D(pDxManager_->GetDxDevice(), metadata);
	if (!newResource.Get()) {
		Console::LogError("[Reload Failed] [Texture] - Don't Create DxResource: \"" + _filepath + "\"");
		return {};
	}

	{	/// 読み込む前にテクスチャの情報をログに出力する
		Console::Log("[Reload Texture Info] Path: \"" + _filepath + "\"");
		Console::Log(" - Width: " + std::to_string(metadata.width));
		Console::Log(" - Height: " + std::to_string(metadata.height));
		Console::Log(" - MipLevels: " + std::to_string(metadata.mipLevels));
		Console::Log(" - Format: " + std::string(magic_enum::enum_name(metadata.format)));
		Console::Log(" - Dimension: " + std::string(magic_enum::enum_name(metadata.dimension)));
	}

	newResource.Get()->SetName(ConvertString(_filepath).c_str());
	texture.dxResource_ = std::move(newResource);
	DxResource intermediateResource = UploadTextureData(texture.dxResource_.Get(), scratchImage);
	pDxManager_->GetDxCommand()->CommandExecuteAndWait();
	pDxManager_->GetDxCommand()->CommandReset();

	/// srv の更新
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

	/// srvの生成
	DxDevice* dxDevice = pDxManager_->GetDxDevice();
	dxDevice->GetDevice()->CreateShaderResourceView(texture.dxResource_.Get(), &srvDesc, texture.srvHandle_->cpuHandle);

	/// texture size
	Vector2 textureSize = { static_cast<float>(metadata.width), static_cast<float>(metadata.height) };
	texture.textureSize_ = textureSize;
	/// テクスチャフォーマット
	texture.srvFormat_ = metadata.format;

	{	/// ログに完了したテクスチャの情報を書き出す
		Console::Log("[Success Reload Texture Info] Path: \"" + _filepath + "\"");
		Console::Log(" - DescriptorIndex: " + std::to_string(texture.srvHandle_->descriptorIndex));
	}
	return std::move(texture);
}


Texture AssetLoaderT<Texture>::Reload3DTexture(const std::string& _filepath, Texture* _src) {
	/// ----- DDSファイルの読み込み ----- ///
	Texture texture = *_src; // 元のテクスチャをコピー
	/// スクラッチイメージを読み込み、使用可能かチェック
	DirectX::ScratchImage scratchImage = LoadScratchImage3D(_filepath);
	if (scratchImage.GetImageCount() == 0) {
		Console::LogError("[Reload Failed] [Texture DDS] - Failed to load DDS file: \"" + _filepath + "\"");
		return {};
	}
	/// metadataは3Dテクスチャでないといけない
	const DirectX::TexMetadata& metadata = scratchImage.GetMetadata();
	if (metadata.dimension != DirectX::TEX_DIMENSION_TEXTURE3D) {
		Console::LogError("[Reload Failed] [Texture DDS] - Not a 3D texture: \"" + _filepath + "\"");
		return {};
	}

	/// width height depthのチェック
	if (metadata.width == 0 || metadata.height == 0 || metadata.depth == 0) {
		Console::LogError("[Reload Failed] [Texture DDS] - Invalid dimensions: \"" + _filepath + "\"");
		return {};
	}

	/// texture resource の更新
	DxResource newResource = CreateTextureResource3D(pDxManager_->GetDxDevice(), metadata);
	if (!newResource.Get()) {
		Console::LogError("[Reload Failed] [Texture DDS] - Don't Create DxResource: \"" + _filepath + "\"");
		return {};
	}

	newResource.Get()->SetName(ConvertString(_filepath).c_str());
	texture.dxResource_ = std::move(newResource);
	DxResource intermediateResource = UploadTextureData(texture.dxResource_.Get(), scratchImage);

	pDxManager_->GetDxCommand()->CommandExecuteAndWait();
	pDxManager_->GetDxCommand()->CommandReset();

	/// テクスチャサイズ
	Vector2 textureSize = { static_cast<float>(metadata.width), static_cast<float>(metadata.height) };
	texture.textureSize_ = textureSize;
	texture.depth_ = static_cast<uint32_t>(metadata.depth);
	/// テクスチャフォーマット
	texture.srvFormat_ = metadata.format;
	return std::move(texture);
}


DirectX::ScratchImage AssetLoaderT<Texture>::LoadScratchImage2D(const std::string& _filepath) {
	/// テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring          filePathW = ConvertString(_filepath);
	if (_filepath.ends_with(".dds")) {
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

DirectX::ScratchImage AssetLoaderT<Texture>::LoadScratchImage3D(const std::string& _filepath) {
	// DDS ファイル専用
	if (!_filepath.ends_with(".dds")) {
		Console::LogError("LoadScratchImage3D: Only DDS files are supported for Texture3D.");
		return DirectX::ScratchImage{};
	}

	std::wstring filePathW = ConvertString(_filepath);
	DirectX::ScratchImage image;

	HRESULT hr = DirectX::LoadFromDDSFile(
		filePathW.c_str(),
		DirectX::DDS_FLAGS_NONE,
		nullptr,
		image
	);
	if (FAILED(hr)) {
		Console::LogError("[Load Failed] Texture3D DDS: \"" + _filepath + "\"");
		return DirectX::ScratchImage{};
	}

	// Texture3D かどうかチェック
	const DirectX::TexMetadata& meta = image.GetMetadata();
	if (meta.dimension != DirectX::TEX_DIMENSION_TEXTURE3D) {
		Console::LogError("[Load Failed] File is not a Texture3D DDS: \"" + _filepath + "\"");
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
			Console::LogWarning("[GenerateMipMaps Failed] Using original image: \"" + _filepath + "\"");
			mipImages = std::move(image);
		}
	}

	return mipImages;
}


DxResource AssetLoaderT<Texture>::CreateTextureResource2D(DxDevice* _dxDevice, const DirectX::TexMetadata& _metadata) {
	/// metadataを基にResourceの設定
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

	/// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;	//- 細かい設定を行う

	/// Resourceを生成
	DxResource dxResource;
	dxResource.CreateCommittedResource(
		_dxDevice,
		&heapProperties, D3D12_HEAP_FLAG_NONE,
		&desc, D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr
	);

	return dxResource;
}

DxResource AssetLoaderT<Texture>::CreateTextureResource3D(DxDevice* _dxDevice, const DirectX::TexMetadata& _metadata) {
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

DxResource AssetLoaderT<Texture>::UploadTextureData(ID3D12Resource* _texture, const DirectX::ScratchImage& _mipScratchImage) {
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