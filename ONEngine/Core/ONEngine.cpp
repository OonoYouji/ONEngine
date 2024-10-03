#include "ONEngine.h"

#include <memory>


namespace {
	std::unique_ptr<System> gSystem;
}


/// ===================================================
/// ↓ ONEngine
/// ===================================================

void ONEngine::Initialize() {
	gSystem.reset(new System);
	gSystem->Initialize();
}

void ONEngine::Finalize() {
	gSystem->Finalize();
}

ONE::DxCommon* ONEngine::GetDxCommon() {
	return gSystem->dxCommon_.get();
}

ONE::WinApp* ONEngine::GetWinApp() {
	return gSystem->winApp_.get();
}



/// ===================================================
/// ↓ System
/// ===================================================

void System::Initialize() {

	dxCommon_.reset(new ONE::DxCommon());
	dxCommon_->Initialize();
	
	winApp_.reset(new ONE::WinApp());
	winApp_->Initialize(L"DirectXGame ver.2.0");

}

void System::Finalize() {
	winApp_->Finalize();
	dxCommon_->Finalize();
}
