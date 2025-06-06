#include "DxDevice.h"

/// lib
#include "Engine/Core/Utility/Tools/Log.h"
#include "Engine/Core/Utility/Tools/Assert.h"


/// comment
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")


DxDevice::DxDevice() {}
DxDevice::~DxDevice() {}

void DxDevice::Initialize() {

	HRESULT hr = S_FALSE;

	/// ---------------------------------------------------
	/// dxgiFactory
	/// ---------------------------------------------------
	hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	Assert(SUCCEEDED(hr), "Factory generation failed.");


	/// ---------------------------------------------------
	/// useAdapter 
	/// ---------------------------------------------------
	for(UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_)) != DXGI_ERROR_NOT_FOUND; i++) {

		DXGI_ADAPTER_DESC3 desc;
		hr = useAdapter_->GetDesc3(&desc);
		Assert(SUCCEEDED(hr), "Failed to retrieve Desc.");

		/// ソフトウェアアダプタでなければ採用
		if(!(desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			Console::Log(std::format(L"Use Adapter:{}", desc.Description));
			break;
		}

		/// ソフトフェアアダプタのときは無視
		useAdapter_ = nullptr;
	}

	Assert(useAdapter_ != nullptr, "Adapter used was nullptr.");



	/// ---------------------------------------------------
	/// device
	/// ---------------------------------------------------

	const char*       featureLevelStaring[] = { "12.2", "12.1","12.0" };
	D3D_FEATURE_LEVEL featureLevels[]       = {
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0
	};

	for(size_t i = 0; i < _countof(featureLevels); i++) {
		hr = D3D12CreateDevice(useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
		/// 指定した機能ベルで生成できたか確認
		if(SUCCEEDED(hr)) {
			Console::Log(std::format("FeatureLevel : {}", featureLevelStaring[i]));
			break;
		}
	}

	/// 生成できたか確認、生成出来ていたらログ出力する
	Assert(device_ != nullptr, "Device creation failed.");
	Console::Log("dx device create success!!");



	/// ---------------------------------------------------
	/// device10
	/// ---------------------------------------------------

	hr = device_->QueryInterface(IID_PPV_ARGS(&device10_));
	Assert(SUCCEEDED(hr), "Device10 creation failed.");

}

IDXGIFactory7* DxDevice::GetFactory() const {
	return dxgiFactory_.Get();
}

ID3D12Device* DxDevice::GetDevice() const {
	return device_.Get();
}

ID3D12Device10* DxDevice::GetDevice10() const {
	return device10_.Get();
}
