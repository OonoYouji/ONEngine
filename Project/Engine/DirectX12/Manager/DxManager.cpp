#include "DxManager.h"

DxManager::DxManager() {}

DxManager::~DxManager() {}

void DxManager::Initialize() {

	/// deviceの初期化
	dxDevice_.reset(new DxDevice());
	dxDevice_->Initialize();

	/// commandの初期化
	dxCommand_.reset(new DxCommand());
	dxCommand_->Initialize(dxDevice_.get());

}

void DxManager::Finalize() {}
