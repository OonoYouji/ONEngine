#include <ImGuiManager.h>

#include <Core/ONEngine.h>
#include <DxCommon.h>
#include <DxDescriptor.h>
#include <DxCommand.h>
#include <DxDoubleBuffer.h>

#include <WinApp.h>


bool IsMouseInWindow(HWND hwnd) {
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(hwnd, &pt);

	RECT rect;
	GetClientRect(hwnd, &rect);
	return (pt.x >= 0 && pt.x < rect.right && pt.y >= 0 && pt.y < rect.bottom);
}



/// ===================================================
/// インスタンス確保
/// ===================================================
ImGuiManager* ImGuiManager::GetInstance() {
	static ImGuiManager instance;
	return &instance;
}


/// ===================================================
/// 初期化
/// ===================================================
void ImGuiManager::Initialize(ONE::WinApp* winApp, ONE::DxCommon* dxCommon) {

	pWinApp_ = winApp;
	dxCommon_ = dxCommon;
	dxDescriptor_ = dxCommon->GetDxDescriptor();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& imGuiIO = ImGui::GetIO();
	imGuiIO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;


	ImGui::StyleColorsDark();
	StyleSetting();
	ImGui_ImplWin32_Init(pWinApp_->GetHWND());
	ImGui_ImplDX12_Init(
		dxCommon->GetDevice(),
		ONE::DxDoubleBuffer::kBufferCount,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		dxDescriptor_->GetSRV(),
		dxDescriptor_->GetSrvCpuHandle(),
		dxDescriptor_->GetSrvGpuHandle()
	);

	dxDescriptor_->AddSrvUsedCount();
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
}


/// ===================================================
/// 終了処理
/// ===================================================
void ImGuiManager::Finalize() {

	dxDescriptor_ = nullptr;

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

}


/// ===================================================
/// 毎フレーム最初に行う
/// ===================================================
void ImGuiManager::BeginFrame() {

	HWND activeWindow = GetForegroundWindow();
	ImGui::NewFrame();

		isActive_ = true;
	if(!IsMouseInWindow(ONEngine::GetWinApps().at("Debug")->GetHWND())) {
		ImGui::BeginDisabled();
	}

	

	//dxDescriptor_->SetSRVHeap(dxCommon_->GetDxCommand()->GetList());

}


/// ===================================================
/// 毎フレーム最後に行う
/// ===================================================
void ImGuiManager::EndFrame() {
	if(!IsMouseInWindow(ONEngine::GetWinApps().at("Debug")->GetHWND())) {
		ImGui::EndDisabled();
	}
	//dxDescriptor_->SetSRVHeap(dxCommon_->GetDxCommand()->GetList());

	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon_->GetDxCommand()->GetList());

}


/// ===================================================
/// imguiの見た目設定
/// ===================================================
void ImGuiManager::StyleSetting() {
	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_FrameBg] = ColorToVec4(0x3e3e3e8a);
	style.Colors[ImGuiCol_TitleBgActive] = ColorToVec4(0x4c4c4cff);
	style.Colors[ImGuiCol_CheckMark] = ColorToVec4(0xffffffff);
	style.Colors[ImGuiCol_ScrollbarGrab] = ColorToVec4(0xffffffff);
	style.Colors[ImGuiCol_Button] = ColorToVec4(0x3e3e3e8a);
	style.Colors[ImGuiCol_Header] = ColorToVec4(0xcccccc4f);
	style.Colors[ImGuiCol_Tab] = ColorToVec4(0x333333ff);
	style.Colors[ImGuiCol_TabActive] = ColorToVec4(0xaaaaaaff);

}


/// ===================================================
/// カラーコードをImGui::Vector4に変換する
/// ===================================================
ImVec4 ImGuiManager::ColorToVec4(uint32_t color) {
	return {
		((color >> 24) & 0xff) / 255.0f, //- R
		((color >> 16) & 0xff) / 255.0f, //- G
		((color >> 8) & 0xff) / 255.0f,  //- B
		((color >> 0) & 0xff) / 255.0f   //- A
	};
}
