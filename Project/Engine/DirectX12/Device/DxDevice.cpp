#include "DxDevice.h"

/// lib
#include "Lib/DebugTools/Log.h"
#include "Lib/DebugTools/Assert.h"


/// comment
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")


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
			Log(std::format(L"Use Adapter:{}", desc.Description));
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
			Log(std::format("FeatureLevel : {}", featureLevelStaring[i]));
			break;
		}
	}

	/// 生成できたか確認、生成出来ていたらログ出力する
	Assert(device_ != nullptr, "Device creation failed.");
	Log("dx device create success!!");
}
