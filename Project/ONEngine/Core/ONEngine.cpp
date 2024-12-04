#include "ONEngine.h"

#include <memory>

/// engine
#include "Input/Input.h"
#include "FrameManager/Time.h"
#include "FrameManager/FrameFixation.h"
#include "ImGuiManager/ImGuiManager.h"
#include "CommandManager/CommandLineInterface.h"
#include "AudioManager/AudioManager.h"
#include "CollisionManager/CollisionManager.h"
#include "Objects/Camera/Manager/CameraManager.h"
#include "SceneManager/SceneManager.h" 

#include "GraphicManager/TextureManager/TextureManager.h"
#include "GraphicManager/ModelManager/ModelManager.h"
#include "GraphicManager/RenderTextureManager/RenderTextureManager.h"
#include "GraphicManager/Drawer/Sprite/SpriteManager.h"
#include "GraphicManager/Drawer/LineDrawer/Line2D.h"
#include "GraphicManager/Drawer/LineDrawer/Line3D.h"

/// graphics engine
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxShaderCompiler.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDebug.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDepthStencil.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDoubleBuffer.h"

#include "GraphicManager/GraphicsEngine/DirectX12/DxCommand.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDevice.h"

/// component 
#include "ComponentManager/RenderComponentInitializer/RenderComponentInitializer.h"

/// objects
#include "Objects/Camera/DebugCamera.h"


namespace {
	std::unique_ptr<System> gSystem;
	std::unique_ptr<RenderComponentInitializer> gRenderComponentInitializer;

	SceneManager*         sceneManager      = SceneManager::GetInstance();
	ModelManager*         modelManager      = ModelManager::GetInstance();
	SpriteManager*        spriteManager     = SpriteManager::GetInstance();
	TextureManager*       textureManager    = TextureManager::GetInstance();
	AudioManager*         audioManager      = AudioManager::GetInstance();
	CameraManager*        cameraManager     = CameraManager::GetInstance();
	GameObjectManager*    gameObjectManager = GameObjectManager::GetInstance();
	CollisionManager*     collisionManager  = CollisionManager::GetInstance();
	Line2D*               line2d            = Line2D::GetInstance();
	RenderTextureManager* renderTexManager  = RenderTextureManager::GetInstance();

}


/// ===================================================
/// ↓ ONEngine
/// ===================================================

void ONEngine::Initialize(
	const wchar_t* windowName, bool isCreateGameWindow, bool isFrameFixation, uint32_t maxFrame) {

	gRenderComponentInitializer.reset(new RenderComponentInitializer());
	gSystem.reset(new System);
	gSystem->Initialize(windowName, isCreateGameWindow, isFrameFixation, maxFrame);

}

void ONEngine::Finalize() {
	gSystem->Finalize();
}

void ONEngine::Update() {
	gSystem->Update();
}

void ONEngine::PreDraw() {
	gSystem->PreDraw();
}

void ONEngine::PostDraw() {
	gSystem->PostDraw();
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

bool ONEngine::GetIsRunning() {
	return gSystem->isRunning_;
}

void ONEngine::SetIsRunning(bool _isRunning) {
	gSystem->isRunning_ = _isRunning;
}


/// ===================================================
/// ↓ System
/// ===================================================

void System::Initialize(
	const wchar_t* windowName, bool isCreateGameWindow, bool isFrameFixation, uint32_t maxFrameRate) {


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
	
	input_ = Input::GetInsatnce();
	input_->Initialize(ONEngine::GetMainWinApp());

	time_  = Time::GetInstance();
	frameFixation_.reset(new  FrameFixation);
	frameFixation_->Initialize(isFrameFixation, maxFrameRate);

	sceneManager_ = SceneManager::GetInstance();


#ifdef _DEBUG /// release not building  initializing
	/// console initialize
	consoleManager_ = ConsoleManager::GetInstance();
	consoleManager_->Initialize();

	imguiManager_ = ImGuiManager::GetInstance();
	imguiManager_->Initialize(mainWindow_, dxCommon_.get());
#endif // _DEBUG


	modelManager->Initialize();
	spriteManager->Initialize();
	line2d->Initialize();
	Line3D::SInitialize(ONEngine::GetDxCommon()->GetDxCommand()->GetList());
	audioManager->Initialize();

	textureManager->Load("uvChecker", "uvChecker.png");
	textureManager->Load("white2x2", "white2x2.png");

	/// render texture imgui用を作成
	renderTexManager->Initialize(
		ONEngine::GetDxCommon()->GetDxCommand()->GetList(),
		ONEngine::GetDxCommon()
	);

	/// rendering componentの初期化
	gRenderComponentInitializer->Initialize();

	collisionManager->Initialize();

	/// game object manager の初期化
	gameObjectManager->Initialize();
	CommandLineInterface::GetInstance()->Initialize();

	DebugCamera* debugCamera = new DebugCamera();
	debugCamera->Initialize();
	debugCamera->SetPosition({ -1.48f, 0.9f, -14.16f });
	debugCamera->SetRotate({ 0.066f, 0.0f, 0.0f });
	debugCamera->BaseUpdate();

	time_->Update();
}

void System::Finalize() {

	if(!ONEngine::GetDxCommon()->IsGpuExeEnded()) {
		Assert(false, "gpu was still running");
	}

	CommandLineInterface::GetInstance()->Finalize();

	/// rendering componentの終了処理を行う
	gRenderComponentInitializer->Finalize();

	renderTexManager->Finalize();

	sceneManager->Finalize();
	cameraManager->Finalize();
	gameObjectManager->Finalize();
	collisionManager->Finalize();

	Line3D::SFinalize();
	line2d->Finalize();
	audioManager->Finalize();
	spriteManager->Finalize();
	modelManager->Finalize();

	textureManager->Finalize();

	/// ===================================================
	/// object finalizing...
	/// ===================================================

	sceneManager_ = nullptr;

	input_->Finalize();
	input_ = nullptr;

	frameFixation_.reset();
	time_  = nullptr;

#ifdef _DEBUG /// release not building objects
	imguiManager_->Finalize();
	imguiManager_ = nullptr;

	consoleManager_ = nullptr;
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

	/// ===================================================
	/// ↓ engine update...
	/// ===================================================

	input_->Update();
	time_->Update();

#ifdef _DEBUG /// release not building objects...

	imguiManager_->BeginFrame();
	consoleManager_->Update();

#endif // _DEBUG /// release not building objects...

	cameraManager->Update();

}

void System::PreDraw() {
	for(auto& win : winApps_) {
		win.second->PreDraw();
	}

	ID3D12GraphicsCommandList* pCommandList = dxCommon_->GetDxCommand()->GetList();
	dxCommon_->GetSRVDescriptorHeap()->BindToCommandList(pCommandList);
}

void System::PostDraw() {

#ifdef _DEBUG
	imguiManager_->EndFrame();
	winApps_.at("Debug")->PostDraw(ImGuiManager::GetInstance()->GetRenderTexture());
#endif // _DEBUG
	winApps_.at("Game")->PostDraw(sceneManager_->GetFinalRenderTex());

	ONEngine::GetDxCommon()->CommandExecution();
	for(auto& win : winApps_) {
		win.second->Present();
	}
	ONEngine::GetDxCommon()->GetDxCommand()->Reset();

	frameFixation_->Fixation();

}

