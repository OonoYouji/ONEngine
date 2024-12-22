#include "DxManager.h"

DxManager::DxManager() {}

DxManager::~DxManager() {}

void DxManager::Initialize() {

	dxDevice_.reset(new DxDevice());
	dxDevice_->Initialize();

}

void DxManager::Finalize() {}
