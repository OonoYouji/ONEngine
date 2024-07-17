#include <DxCommon.h>

#include <cassert>

#include <Logger.h>
#include <DxDevice.h>
#include <DxCommand.h>


#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")


/// ===================================================
/// インスタンス確保
/// ===================================================
ONE::DxCommon* ONE::DxCommon::GetInstance() {
	static DxCommon instance;
	return &instance;
}


/// ===================================================
/// 初期化
/// ===================================================
void ONE::DxCommon::Initialize() {

	device_.reset(new DxDevice());
	device_->Initialize();

	command_.reset(new DxCommand());
	command_->Initialize(device_->GetDevice());

}


/// ===================================================
/// 終了処理
/// ===================================================
void ONE::DxCommon::Finalize() {

	command_.reset();
	device_.reset();

}


