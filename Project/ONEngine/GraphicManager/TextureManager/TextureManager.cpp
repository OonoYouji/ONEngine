#include "TextureManager.h"

#include <DirectXTex.h>

#include <d3dx12.h>

#include <Core/ONEngine.h>

#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDevice.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommand.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDescriptor.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxResourceCreator.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxBarrierCreator.h"


/// ===================================================
/// 無記名名前空間 : 関数の定義
/// ===================================================
#pragma region namespace methods
namespace {


	/// ===================================================
	/// stringをwstringに変換
	/// ===================================================
	std::wstring ConvertString(const std::string& str) {
		if(str.empty()) {
			return std::wstring();
		}

		auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
		if(sizeNeeded == 0) {
			return std::wstring();
		}
		std::wstring result(sizeNeeded, 0);
		MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
		return result;
	}


	/// ===================================================
	/// Textureを解析
	/// ===================================================
	DirectX::ScratchImage LoadTexture(const std::string& filePath) {
		///- テクスチャファイルを読んでプログラムで扱えるようにする
		DirectX::ScratchImage image{};
		std::wstring filePathW = ConvertString(filePath);
		HRESULT result = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
		assert(SUCCEEDED(result));

		///- mipMapの作成
		DirectX::ScratchImage mipImages{};
		result = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
		assert(SUCCEEDED(result));

		return mipImages;
	}


	/// ===================================================
	/// 
	/// ===================================================
	ComPtr<ID3D12Resource> CreataTextureResouece(ID3D12Device* device, const DirectX::TexMetadata& metadata) {


		/// --------------------------------------
		/// ↓ metadataを基にResourceの設定
		/// --------------------------------------
		D3D12_RESOURCE_DESC desc{};
		desc.Width = UINT(metadata.width);		//- textureの幅
		desc.Height = UINT(metadata.height);	//- textureの高さ
		desc.MipLevels = UINT16(metadata.mipLevels);		//- mipmapの数
		desc.DepthOrArraySize = UINT16(metadata.arraySize);	//- 奥行き or 配列Textureの配列数
		desc.Format = metadata.format;	//- TextureのFormat
		desc.SampleDesc.Count = 1;	//- サンプリングカウント; 1固定
		desc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);	//- Textureの次元数
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		/// --------------------------------------
		/// ↓ 利用するHeapの設定
		/// --------------------------------------

		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;	//- 細かい設定を行う


		/// --------------------------------------
		/// ↓ Resourceを生成
		/// --------------------------------------

		ComPtr<ID3D12Resource> resource;
		HRESULT result = device->CreateCommittedResource(
			&heapProperties,		//- Heapの設定
			D3D12_HEAP_FLAG_NONE,	//- Heapの特殊な設定
			&desc,					//- Resourceの設定
			D3D12_RESOURCE_STATE_COPY_DEST,	//- 初回のResourceState; Textureは基本読むだけ
			nullptr,				//- Clear最適値; 使わないのでnullptr
			IID_PPV_ARGS(&resource)
		);
		assert(SUCCEEDED(result));

		return resource;
	}


	[[nodiscard]]
	ComPtr<ID3D12Resource> UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages) {

		ID3D12Device* device = ONEngine::GetDxCommon()->GetDevice();
		ID3D12GraphicsCommandList* commandList = ONEngine::GetDxCommon()->GetDxCommand()->GetList();

		std::vector<D3D12_SUBRESOURCE_DATA> subresources;
		DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
		uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, static_cast<UINT>(subresources.size()));
		ComPtr<ID3D12Resource> intermediateResource = ONE::DxResourceCreator::CreateResource(intermediateSize);

		UpdateSubresources(commandList, texture, intermediateResource.Get(), 0, 0, static_cast<UINT>(subresources.size()), subresources.data());
		ONE::DxBarrierCreator::CreateBarrier(texture, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);

		return intermediateResource;

	}


} ///- namespace
#pragma endregion



/// ===================================================
/// static objects initialize
/// ===================================================

const std::string TextureManager::kDirectoryPath_ = "./Resources/Textures/";





/// ===================================================
/// インスタンス確保
/// ===================================================
TextureManager* TextureManager::GetInstance() {
	static TextureManager instance;
	return &instance;
}


/// ===================================================
/// 終了処理
/// ===================================================
void TextureManager::Finalize() {
	textures_.clear();
}


/// ===================================================
/// テクスチャのゲッタ
/// ===================================================
const Texture& TextureManager::GetTexture(const std::string& name) const {
	return textures_.at(name);
}


/// ===================================================
/// テクスチャの読み込み
/// ===================================================
void TextureManager::Load(const std::string& texName, const std::string& filePath) {

	if(textures_.find(texName) != textures_.end()) {
		return; /// すでに存在している
	}

	Texture newTexture{};
	DirectX::ScratchImage mipImages = LoadTexture(kDirectoryPath_ + filePath);
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	newTexture.resource_ = CreataTextureResouece(ONEngine::GetDxCommon()->GetDevice(), metadata);
	ComPtr<ID3D12Resource> intermediateResource = UploadTextureData(newTexture.resource_.Get(), mipImages);


	ONEngine::GetDxCommon()->ExecuteCommands();
	intermediateResource.Reset();

	///- metadataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	///- srvHandleの取得
	ONE::DxDescriptor* descriptor = ONEngine::GetDxCommon()->GetDxDescriptor();
	newTexture.cpuHandle_ = descriptor->GetSrvCpuHandle();
	newTexture.gpuHandle_ = descriptor->GetSrvGpuHandle();
	descriptor->AddSrvUsedCount();

	///- srvの生成
	ID3D12Device* device = ONEngine::GetDxCommon()->GetDevice();
	device->CreateShaderResourceView(newTexture.resource_.Get(), &srvDesc, newTexture.cpuHandle_);

	textures_[texName] = newTexture;

}

void TextureManager::AddTexture(const std::string& name, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle) {

	if(textures_.find(name) != textures_.end()) {
		//assert(false);
		return;
	}

	Texture newTexture(cpuHandle, gpuHandle);
	textures_[name] = newTexture;
}
