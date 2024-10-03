#include "ONEngine.h"

#include <memory>

#include <Input.h>

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

void ONEngine::Update() {
	gSystem->Update();
}

ONE::DxCommon* ONEngine::GetDxCommon() {
	return gSystem->dxCommon_.get();
}

ONE::WinApp* ONEngine::GetMainWinApp() {
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
	gameWin->Initialize(L"Game", nullptr);
	
	auto& debugWin = winApps_["Debug"];
	debugWin.reset(new ONE::WinApp());
	debugWin->Initialize(L"Debug", nullptr);

	/// main window のポインタ
	mainWindow_ = gameWin.get();

}

void System::Finalize() {
	for(auto& win : winApps_) {
		win.second->Finalize();
	}
	dxCommon_->Finalize(); 
}

void System::Update() {
	/*HWND activeWindow = GetForegroundWindow();
	for(auto& win : winApps_) {
		if(activeWindow == win.second->GetHWND()) {
			mainWindow_ = win.second.get();
			return;
		}
	}*/

	if(Input::TriggerKey(KeyCode::Space)) {
		auto& debugWin = winApps_["Debug"];
		// 子ウィンドウのスタイルを変更
		SetWindowLongPtr(debugWin->GetHWND(), GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
		SetParent(debugWin->GetHWND(), nullptr); // 親ウィンドウから切り離す
		SetWindowPos(debugWin->GetHWND(), nullptr, 100, 100, 640, 480, SWP_SHOWWINDOW);
	}



}
