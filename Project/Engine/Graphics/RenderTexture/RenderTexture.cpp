#include "RenderTexture.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"

void RenderTexture::Initialize(DXGI_FORMAT _format, const Vector4& _clearColor, const std::string& _name, DxManager* _dxManager, GraphicsResourceCollection* _resourceCollection) {
	clearColor_ = _clearColor;

	{	/// textureの作成
		std::unique_ptr<Texture> rtvTexture = std::make_unique<Texture>();
		texture_ = rtvTexture.get();
		_resourceCollection->AddTexture(_name, std::move(rtvTexture));
	}

	/// 必要なオブジェクトの取得
	DxDevice* dxDevice = _dxManager->GetDxDevice();
	DxSRVHeap* dxSRVHeap = _dxManager->GetDxSRVHeap();
	DxRTVHeap* dxRTVHeap = _dxManager->GetDxRTVHeap();
	DxResource& renderTextureResource = texture_->GetDxResource();

	/// render texture resourceの作成
	renderTextureResource.CreateRenderTextureResource(
		dxDevice, Vector2(1280.0f, 720.0f), _format, _clearColor
	);

	uint32_t rtvHeapIndex = dxRTVHeap->Allocate();
	rtvHandle_.cpuHandle = dxRTVHeap->GetCPUDescriptorHandel(rtvHeapIndex);

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = _format;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	dxDevice->GetDevice()->CreateRenderTargetView(renderTextureResource.Get(), &rtvDesc, rtvHandle_.cpuHandle);


	/// shader resource viewの作成
	texture_->CreateEmptySRVHandle();
	texture_->SetSRVDescriptorIndex(dxSRVHeap->AllocateTexture());
	texture_->SetSRVCPUHandle(dxSRVHeap->GetCPUDescriptorHandel(texture_->GetSRVDescriptorIndex()));
	texture_->SetSRVGPUHandle(dxSRVHeap->GetGPUDescriptorHandel(texture_->GetSRVDescriptorIndex()));

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = _format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = 1;

	dxDevice->GetDevice()->CreateShaderResourceView(renderTextureResource.Get(), &srvDesc, texture_->GetSRVCPUHandle());


	renderTextureResource.CreateBarrier(
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		_dxManager->GetDxCommand()
	);

	/// command exe
	_dxManager->GetDxCommand()->CommandExecute();
	_dxManager->GetDxCommand()->CommandReset();
}

void RenderTexture::SetRenderTarget(DxCommand* _dxCommand, DxDSVHeap* _dxDSVHeap) {
	auto command = _dxCommand->GetCommandList();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = _dxDSVHeap->GetCPUDescriptorHandel(0);

	command->OMSetRenderTargets(1, &rtvHandle_.cpuHandle, FALSE, &dsvHandle);
	command->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	command->ClearRenderTargetView(rtvHandle_.cpuHandle, &clearColor_.x, 0, nullptr);
}

void RenderTexture::SetRenderTarget(DxCommand* _dxCommand, DxDSVHeap* _dxDSVHeap, const std::vector<std::unique_ptr<class RenderTexture>>& _others) {
	auto command = _dxCommand->GetCommandList();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = _dxDSVHeap->GetCPUDescriptorHandel(0);

	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles;
	for (auto& rt : _others) {
		rtvHandles.push_back(rt->rtvHandle_.cpuHandle);
	}

	command->OMSetRenderTargets(static_cast<UINT>(rtvHandles.size()), rtvHandles.data(), FALSE, &dsvHandle);
	command->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	for (auto& rt : _others) {
		command->ClearRenderTargetView(rt->rtvHandle_.cpuHandle, &rt->clearColor_.x, 0, nullptr);
	}
}

void RenderTexture::CreateBarrierRenderTarget(DxCommand* _dxCommand) {
	texture_->GetDxResource().CreateBarrier(
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		_dxCommand
	);
}

void RenderTexture::CreateBarrierPixelShaderResource(DxCommand* _dxCommand) {
	texture_->GetDxResource().CreateBarrier(
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		_dxCommand
	);
}



/// ///////////////////////////////////////////////////
/// UAVTexture
/// ///////////////////////////////////////////////////

UAVTexture::UAVTexture() {}
UAVTexture::~UAVTexture() {}

void UAVTexture::Initialize(const std::string& _textureName, DxManager* _dxManager, GraphicsResourceCollection* _resourceCollection) {
	std::unique_ptr<Texture> uavTexture = std::make_unique<Texture>();
	texture_ = uavTexture.get();
	_resourceCollection->AddTexture(_textureName, std::move(uavTexture));

	/// 必要なオブジェクトの取得
	DxDevice* dxDevice = _dxManager->GetDxDevice();
	DxSRVHeap* dxSRVHeap = _dxManager->GetDxSRVHeap();
	DxResource& uavTextureResource = texture_->GetDxResource();

	/// UAV texture resourceの作成
	uavTextureResource.CreateUAVTextureResource(
		dxDevice, Vector2(1280.0f, 720.0f), DXGI_FORMAT_R32G32B32A32_FLOAT
	);

	texture_->CreateEmptyUAVHandle();
	texture_->SetUAVDescriptorIndex(dxSRVHeap->AllocateTexture());
	texture_->SetUAVCPUHandle(dxSRVHeap->GetCPUDescriptorHandel(texture_->GetUAVDescriptorIndex()));
	texture_->SetUAVGPUHandle(dxSRVHeap->GetGPUDescriptorHandel(texture_->GetUAVDescriptorIndex()));

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	uavDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	dxDevice->GetDevice()->CreateUnorderedAccessView(uavTextureResource.Get(), nullptr, &uavDesc, texture_->GetUAVCPUHandle());

	/// shader resource viewの作成
	texture_->CreateEmptySRVHandle();
	texture_->SetSRVDescriptorIndex(dxSRVHeap->AllocateTexture());
	texture_->SetSRVCPUHandle(dxSRVHeap->GetCPUDescriptorHandel(texture_->GetSRVDescriptorIndex()));
	texture_->SetSRVGPUHandle(dxSRVHeap->GetGPUDescriptorHandel(texture_->GetSRVDescriptorIndex()));

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = 1;

	dxDevice->GetDevice()->CreateShaderResourceView(uavTextureResource.Get(), &srvDesc, texture_->GetSRVCPUHandle());

	uavTextureResource.CreateBarrier(
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		_dxManager->GetDxCommand()
	);

	/// command exe
	_dxManager->GetDxCommand()->CommandExecute();
	_dxManager->GetDxCommand()->CommandReset();
}
