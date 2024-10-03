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
	return gSystem->mainWindow_;
}

const std::unordered_map<std::string, std::unique_ptr<ONE::WinApp>>& ONEngine::GetWinApps() {
	return gSystem->winApps_;
}



/// ===================================================
/// ↓ System
/// ===================================================

void System::Initialize() {

	dxCommon_.reset(new ONE::DxCommon());
	dxCommon_->Initialize();
	

	auto& gameWin = winApps_["Game"];
	gameWin.reset(new ONE::WinApp());
	gameWin->Initialize(L"Game");
	
	auto& debugWin = winApps_["Debug"];
	debugWin.reset(new ONE::WinApp());
	debugWin->Initialize(L"Debug");

	/// main window のポインタ
	mainWindow_ = debugWin.get();

}

void System::Finalize() {
	for(auto& win : winApps_) {
		win.second->Finalize();
	}
	dxCommon_->Finalize(); 
}
