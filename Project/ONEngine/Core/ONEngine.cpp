#include "ONEngine.h"

#include <memory>

#include "Input/Input.h"
#include "FrameManager/Time.h"
#include "CommandManager/CommandLineInterface.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxShaderCompiler.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDebug.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDepthStencil.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDoubleBuffer.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDescriptor.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommand.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDevice.h"


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


	/// ===================================================
	/// ↓ engine base initializing...
	/// ===================================================

	/// graphics engine initializing...
	dxCommon_.reset(new ONE::DxCommon());
	dxCommon_->Initialize();


	/// window application initializing...
	auto& gameWin = winApps_["Game"];
	gameWin.reset(new ONE::WinApp());
	gameWin->Initialize(windowName, nullptr);
	
#ifdef _DEBUG /// debug window...

	/// releaseの時は無効
	if(!isCreateGameWindow) {
		gameWin->DestoryGameWindow();
	}

	auto& debugWin = winApps_["Debug"];
	debugWin.reset(new ONE::WinApp());
	debugWin->Initialize(L"Debug", nullptr);

	mainWindow_   = debugWin.get();
	activeWindow_ = debugWin.get();
#else /// game window...
	mainWindow_   = gameWin.get();
	activeWindow_ = gameWin.get();
#endif // DEBUG


	/// ===================================================
	/// ↓ engine app initializing
	/// ===================================================

	/// instance get...
	input_ = Input::GetInsatnce();
	time_  = Time::GetInstance();

	/// initializing...
	input_->Initialize(ONEngine::GetMainWinApp());


#ifdef _DEBUG /// release not building  initializing
	/// console initialize
	console_.reset(new Console());
	console_->Initialize();

	/// cli initialize
	//CommandLineInterface* cli = CommandLineInterface::GetInstance();
	//cli->Initialize();
#endif // _DEBUG

}

void System::Finalize() {
	//CommandLineInterface* cli = CommandLineInterface::GetInstance();
	//cli->Finalize();

	/// ===================================================
	/// object finalizing...
	/// ===================================================

	input_->Finalize();


	input_ = nullptr;
	time_  = nullptr;

#ifdef _DEBUG /// release not building objects
	console_.reset();
#endif // _DEBUG /// release not building objects



	for(auto& win : winApps_) {
		win.second->Finalize();
	}
	dxCommon_->Finalize(); 
}

void System::Update() {
#ifdef _DEBUG  /// release時にはwindowは一個しかないので初期化時のものをずっと使う
	HWND activeWindow = GetForegroundWindow();
	for(auto& win : winApps_) {
		if(activeWindow == win.second->GetHWND()) {
			activeWindow_ = win.second.get();
			break;
		}
	}
#endif // _DEBUG


	/// 終了処理に移行するか ? return
	isRunning_ = !mainWindow_->ProcessMessage();
	if(!isRunning_) {
		return;
	}

	/// ===================================================
	/// ↓ engine update...
	/// ===================================================

	input_->Update();
	time_->Update();

#ifdef _DEBUG /// release not building objects...

	console_->Update();

#endif // _DEBUG /// release not building objects...


}

