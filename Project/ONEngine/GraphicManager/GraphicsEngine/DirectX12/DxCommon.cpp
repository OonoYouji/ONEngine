#include "DxCommon.h"

#include <cassert>

#include "LoggingManager/Logger.h"

#include "DxDevice.h"
#include "DxCommand.h"
#include "DxDescriptor.h"
#include "DxDoubleBuffer.h"
#include "DxDepthStencil.h"
#include "DxDebug.h"
#include "DxShaderCompiler.h"

#include "GraphicManager/RenderTextureManager/RenderTexture.h"

#include "Scenes/Manager/SceneManager.h"


#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")


/// ===================================================
/// 初期化
/// ===================================================
void ONE::DxCommon::Initialize() {

	debug_.reset(new DxDebug());
	debug_->SetDebugLayer();

	device_.reset(new DxDevice());
	device_->Initialize();

	debug_->Initialize(device_->GetDevice());

	command_.reset(new DxCommand());
	command_->Initialize(device_->GetDevice());

	descriptor_.reset(new DxDescriptor());
	descriptor_->Initialize(device_->GetDevice());

	//doubleBuffer_.reset(new DxDoubleBuffer());
	//doubleBuffer_->Initialize(winApp, device_.get(), descriptor_.get(), command_->GetQueue());

	depthStencil.reset(new DxDepthStencil());
	depthStencil->Initialize(device_->GetDevice());

	shaderCompiler_.reset(new DxShaderCompiler());
	shaderCompiler_->Initialize();


}


/// ===================================================
/// 終了処理
/// ===================================================
void ONE::DxCommon::Finalize() {

	shaderCompiler_.reset();
	depthStencil.reset();
	//doubleBuffer_.reset();
	descriptor_.reset();
	command_.reset();
	device_.reset();

	debug_.reset();

}


void ONE::DxCommon::CommandExecution() {
	command_->Close();
	command_->Execution();
}


/// ===================================================
/// CommandList内にある命令を実行する
/// ===================================================
void ONE::DxCommon::ExecuteCommands() {

	command_->Close();		//- listを閉じる
	command_->Execution();	//- 実行
	command_->Reset();		//- 実行し終わったらlistのリセット	

}

void ONE::DxCommon::SetRenderTarget() {
	//doubleBuffer_->SetRanderTarget(command_.get(), descriptor_.get());
}


/// ===================================================
/// Deviceのゲッタ
/// ===================================================
ID3D12Device* ONE::DxCommon::GetDevice() const {
	return device_->GetDevice();
}

bool ONE::DxCommon::IsGpuExeEnded() {
	return command_->IsExeCommandEnded();;
}

