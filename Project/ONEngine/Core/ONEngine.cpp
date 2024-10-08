#include "ONEngine.h"

#include <memory>

#include "Input/Input.h"
#include "CommandManager/CommandLineInterface.h"

namespace {
	std::unique_ptr<System> gSystem;
}


/// ===================================================
/// ↓ ONEngine
/// ===================================================

void ONEngine::Initialize(const wchar_t* windowName, bool isCreateGameWindow) {
	gSystem.reset(new System);
	gSystem->Initialize(windowName, isCreateGameWindow);
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

ONE::WinApp* ONEngine::GetActiveWinApp() {
	return gSystem->activeWindow_;
}

const std::unordered_map<std::string, std::unique_ptr<ONE::WinApp>>& ONEngine::GetWinApps() {
	return gSystem->winApps_;
}

bool ONEngine::IsRunning() {
	return gSystem->isRunning_;
}



/// ===================================================
/// ↓ System
/// ===================================================

void System::Initialize(const wchar_t* windowName, bool isCreateGameWindow) {

	dxCommon_.reset(new ONE::DxCommon());
	dxCommon_->Initialize();
	

	auto& gameWin = winApps_["Game"];
	gameWin.reset(new ONE::WinApp());
	gameWin->Initialize(windowName, nullptr);
	if(!isCreateGameWindow) {
		gameWin->DestoryGameWindow();
	}
	
	auto& debugWin = winApps_["Debug"];
	debugWin.reset(new ONE::WinApp());
	debugWin->Initialize(L"Debug", nullptr);

	/// main window のポインタ
	mainWindow_   = debugWin.get();
	activeWindow_ = debugWin.get();

	Input::GetInsatnce()->Initialize(ONEngine::GetMainWinApp());


	/// console initialize
	console_.reset(new Console());
	console_->Initialize();

	/// cli initialize
	//CommandLineInterface* cli = CommandLineInterface::GetInstance();
	//cli->Initialize();

}

void System::Finalize() {
	//CommandLineInterface* cli = CommandLineInterface::GetInstance();
	//cli->Finalize();

	for(auto& win : winApps_) {
		win.second->Finalize();
	}
	dxCommon_->Finalize(); 
}

void System::Update() {
	HWND activeWindow = GetForegroundWindow();
	for(auto& win : winApps_) {
		if(activeWindow == win.second->GetHWND()) {
			activeWindow_ = win.second.get();
			break;
		}
	}


	console_->Update();

	isRunning_ = !mainWindow_->ProcessMessage();

}

