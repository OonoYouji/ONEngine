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

	/// srv, rtv, dsvの生成、初期化
	srvDescriptorHeap_.reset(new DxDescriptorHeap<HeapType::CBV_SRV_UAV>(128));
	rtvDescriptorHeap_.reset(new DxDescriptorHeap<HeapType::RTV>(32));
	dsvDescriptorHeap_.reset(new DxDescriptorHeap<HeapType::DSV>(1));

	srvDescriptorHeap_->Initialize(this);
	rtvDescriptorHeap_->Initialize(this);
	dsvDescriptorHeap_->Initialize(this);

	depthStencil.reset(new DxDepthStencil());
	depthStencil->Initialize(device_->GetDevice());

	shaderCompiler_.reset(new DxShaderCompiler());
	shaderCompiler_->Initialize();



}


/// ===================================================
/// 終了処理
/// ===================================================
void ONE::DxCommon::Finalize() {

	srvDescriptorHeap_.reset();
	rtvDescriptorHeap_.reset();
	dsvDescriptorHeap_.reset();

	shaderCompiler_.reset();
	depthStencil.reset();
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


