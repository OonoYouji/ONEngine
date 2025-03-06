#include "ImGuiManager.h"

/// external
#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/Window/WindowManager.h"



ImGuiManager::ImGuiManager(DxManager* _dxManager, WindowManager* _windowManager)
	: dxManager_(_dxManager), windowManager_(_windowManager) {}

ImGuiManager::~ImGuiManager() {
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}



void ImGuiManager::Initialize() {

	DxSRVHeap* dxSRVHeap = dxManager_->GetDxSRVHeap();
	uint32_t   srvDescriptorIndex = dxSRVHeap->AllocateBuffer();


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& imGuiIO = ImGui::GetIO();
	imGuiIO.ConfigFlags = ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_NavEnableKeyboard;
	imGuiIO.Fonts->AddFontFromFileTTF("./Assets/Fonts/FiraMono-Regular_0.ttf", 16.0f);
	imGuiIO.KeyRepeatDelay = 4.145f;
	imGuiIO.KeyRepeatRate = 12.0f;

	ImGui_ImplDX12_InvalidateDeviceObjects();
	ImGui_ImplDX12_CreateDeviceObjects();

	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(windowManager_->GetMainWindow()->GetHwnd());
	ImGui_ImplDX12_Init(
		dxManager_->GetDxDevice()->GetDevice(),
		2, ///< swap chain buffer count
		DXGI_FORMAT_R8G8B8A8_UNORM,
		dxSRVHeap->GetHeap(),
		dxSRVHeap->GetCPUDescriptorHandel(srvDescriptorIndex),
		dxSRVHeap->GetGPUDescriptorHandel(srvDescriptorIndex)
	);

	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();

	debugGameWindow_ = windowManager_->GenerateWindow(L"game", Vec2(1280, 720), WindowManager::WindowType::Sub);
	windowManager_->HideGameWindow(debugGameWindow_);

	LONG style = GetWindowLong(debugGameWindow_->GetHwnd(), GWL_STYLE);
	style &= ~WS_SYSMENU; // システムメニュー（閉じるボタン含む）を無効化
	SetWindowLong(debugGameWindow_->GetHwnd(), GWL_STYLE, style);

	/// main windowの描画関数を登録
	RegisterImguiRenderFunc([this]() {
		ImGui::Begin("main window");
		ImGui::End();
	});


	RegisterImguiRenderFunc([this]() {
		ImGui::Begin("game debug button");

		if (ImGui::Button("start")) {
			isGameDebug_ = true;
			windowManager_->ShowGameWindow(debugGameWindow_);
		}

		if (ImGui::Button("end")) {
			isGameDebug_ = false;
			windowManager_->HideGameWindow(debugGameWindow_);
		}

		ImGui::End();
	});
}

void ImGuiManager::Update() {
	ImGui::NewFrame();

	for (auto& func : imguiRenderFuncs_) {
		func();
	}
}

void ImGuiManager::Draw() {
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(
		ImGui::GetDrawData(),
		dxManager_->GetDxCommand()->GetCommandList()
	);
}

