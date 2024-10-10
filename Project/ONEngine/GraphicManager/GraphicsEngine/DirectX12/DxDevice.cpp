#include "DxDevice.h"

#include <cassert>

#include "LoggingManager/Logger.h"



ONE::DxDevice::DxDevice() {}
ONE::DxDevice::~DxDevice() {
	device_.Reset();
	useAdapter_.Reset();
	dxgiFactory_.Reset();
}


/// ===================================================
/// 初期化
/// ===================================================
void ONE::DxDevice::Initialize() {

	HRESULT hr = S_FALSE;

	/// ---------------------------------------------------
	/// dxgiFactory
	/// ---------------------------------------------------
	hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(hr));


	/// ---------------------------------------------------
	/// useAdapter 
	/// ---------------------------------------------------
	for(UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_)) != DXGI_ERROR_NOT_FOUND; i++) {

		DXGI_ADAPTER_DESC3 desc;
		hr = useAdapter_->GetDesc3(&desc);
		assert(SUCCEEDED(hr));

		///- ソフトウェアアダプタでなければ採用
		if(!(desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			ONE::Logger::ConsolePrint(std::format(L"Use Adapter:{}", desc.Description));
			break;
		}

		///- ソフトフェアアダプタのときは無視
		useAdapter_ = nullptr;
	}

	assert(useAdapter_ != nullptr);



	/// ---------------------------------------------------
	/// device
	/// ---------------------------------------------------

	const char* featureLevelStaring[] = { "12.2", "12.1","12.0" };
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0
	};

	for(size_t i = 0; i < _countof(featureLevels); i++) {
		hr = D3D12CreateDevice(useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
		// 指定した機能れhベルで生成できたか確認
		if(SUCCEEDED(hr)) {
			// 生成後にログ出力
			ONE::Logger::ConsolePrint(std::format("FeatureLevel : {}", featureLevelStaring[i]));
			break;
		}
	}

	///- 生成できたか確認; 生成出来ていたらログ出力する
	assert(device_ != nullptr);
	ONE::Logger::ConsolePrint("Compile create D3D12Device!!!");

}
