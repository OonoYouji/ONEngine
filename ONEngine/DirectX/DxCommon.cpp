#include <DxCommon.h>

#include <cassert>

#include <Logger.h>
#include <DxDevice.h>
#include <DxCommand.h>
#include <DxDescriptor.h>
#include <DxDoubleBuffer.h>


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

	descriptor_.reset(new DxDescriptor());
	descriptor_->Initialize(device_->GetDevice());

	doubleBuffer_.reset(new DxDoubleBuffer());
	doubleBuffer_->Initialize(device_.get(), descriptor_.get(), command_->GetQueue());

}


/// ===================================================
/// 終了処理
/// ===================================================
void ONE::DxCommon::Finalize() {

	doubleBuffer_.reset();
	command_.reset();
	device_.reset();

}

void ONE::DxCommon::PreDraw() {
	doubleBuffer_->ClearBB(command_->GetList());
}

void ONE::DxCommon::PostDraw() {
	
	command_->Close();

	command_->Execution();
	doubleBuffer_->Present();
	command_->Reset();

}


