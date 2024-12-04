#define NOMINMAX
#include "Console.h"

/// std
#include <format>
#include <windows.h>
#include <commdlg.h>
#include <iostream>
#include <locale>
#include <shlwapi.h>

/// engine
#include <Core/ONEngine.h>
#include "GraphicManager/TextureManager/TextureManager.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDescriptorHeap.h"
#include "LoggingManager/Logger.h"

#pragma comment(lib, "shlwapi.lib") // Shlwapi.libをリンク

using namespace ONE;



namespace {
	std::wstring ShowSaveFileDialog() {
		// ファイルダイアログの構造体
		OPENFILENAME ofn;
		wchar_t szFile[260];   // ファイル名を格納するバッファ

		// 構造体の初期化
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = szFile;
		ofn.lpstrFile[0] = L'\0';  // ワイド文字の初期化
		ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]); // 配列のサイズをワイド文字数で計算
		ofn.lpstrFilter = L"All Files\0*.*\0Text Documents\0*.TXT\0"; // フィルタ設定
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.lpstrTitle = L"Save As"; // ダイアログのタイトル
		ofn.Flags = OFN_OVERWRITEPROMPT; // 上書き確認

		// 保存ダイアログを表示
		if(GetSaveFileName(&ofn)) {
			// フルパスからファイル名を取得して返す
			return PathFindFileName(ofn.lpstrFile); // ファイル名のみを返す
		} else {
			return L""; // キャンセルされた場合は空の文字列を返す
		}
	}
}



void Console::Initialize() {

	parentWinFlags_ = 0;
	parentWinFlags_ |= ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar;
	parentWinFlags_ |= ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse;
	parentWinFlags_ |= ImGuiWindowFlags_::ImGuiWindowFlags_NoMove;
	parentWinFlags_ |= ImGuiWindowFlags_::ImGuiWindowFlags_NoResize;
	parentWinFlags_ |= ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar;

	imguiWinFlags_ = 0;
	imguiWinFlags_ |= ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse;
	imguiWinFlags_ |= ImGuiWindowFlags_::ImGuiWindowFlags_NoMove;
	//imguiWinFlags_ |= ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking;


	/// other class pointer 
	pSceneManager_      = SceneManager::GetInstance();
	pGameObjectManager_ = GameObjectManager::GetInstance();
	pCLI_               = CommandLineInterface::GetInstance();
	pVariableManager_   = VariableManager::GetInstance();
}

void Console::Update() {
#ifdef _DEBUG
	ParentWindow();

	Inspector();
	Herarchy();

	Scene();
	Debug();

	Assets();

	VariableGroupArray();
#endif // _DEBUG
}



void Console::ParentWindow() {
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;


	if(!ImGui::Begin("Parent Window", nullptr, parentWinFlags_)) {
		ImGui::End();
		return;
	}


	/// ===================================================
	/// main window menu baer
	/// ===================================================
	if(ImGui::BeginMenuBar()) {

		WindowSettingMenuBar();
		DescriptorHeapsMunuBar();
		CreateGameObjectMenuBar();

		ImGui::EndMenuBar();
	}

	ImGui::DockSpace(ImGui::GetID("DockSpace"));

	if(parentWinFlags_ & ImGuiWindowFlags_NoMove) {
		ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
	}

	if(parentWinFlags_ & ImGuiWindowFlags_NoResize) {
		ImGui::SetWindowSize(ImVec2(1280.0f, 720.0f));
	}


	ImGui::End();
}

void Console::Inspector() {
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	if(!ImGui::Begin("Inspector", nullptr, imguiWinFlags_)) {
		ImGui::End();
		return;
	}

	//pGameObjectManager_->Inspector();


	ImGui::End();
}

void Console::Herarchy() {
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	if(!ImGui::Begin("Heararchy", nullptr, imguiWinFlags_)) {
		ImGui::End();
		return;
	}

	pGameObjectManager_->Hierarchy();

	ImGui::End();
}

void Console::Scene() {
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	if(!ImGui::Begin("Scene", nullptr, imguiWinFlags_)) {
		ImGui::End();
		return;
	}

	PlayControl();


	ImVec2 max = ImGui::GetWindowContentRegionMax();
	ImVec2 min = ImGui::GetWindowContentRegionMin();
	ImVec2 winSize = {
		max.x - min.x,
		max.y - min.y
	};

	///- 大きさの調整
	ImVec2 texSize = winSize;
	if(texSize.x <= texSize.y) {
		///- x優先
		texSize.y = (texSize.x / 16.0f) * 9.0f;
	} else {
		///- y優先
		float x = (texSize.y / 9.0f) * 16.0f;
		if(x < texSize.x) {
			texSize.x = x;
		} else {
			texSize.y = (texSize.x / 16.0f) * 9.0f;
		}
	}

	ImVec2 texPos = {
		winSize.x * 0.5f,
		winSize.y * 0.5f
	};

	texPos.y -= texSize.y / 2.0f;
	texPos.x -= texSize.x / 2.0f;

	texPos.x = std::max(texPos.x, min.x);
	texPos.y = std::max(texPos.y, min.y + 64.0f);

	/// scene gpu handle ptr
	auto renderTex = pSceneManager_->GetFinalRenderTex();
	ImTextureID sceneId = ImTextureID(renderTex->GetSrvGpuHandle().ptr);
	ImTextureID bgId    = ImTextureID(TextureManager::GetInstance()->GetTexture("white2x2").GetGPUHandle().ptr);

	/// scene draw
	ImGui::SetCursorPos(texPos);
	ImGui::Image(bgId, texSize, ImVec2(0,0), ImVec2(1,1), ImVec4(0,0,0,1));
	ImGui::SetCursorPos(texPos);
	ImGui::Image(sceneId, texSize);

	ImGui::End();
}

void Console::Debug() {
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	if(!ImGui::Begin("Debug", nullptr, imguiWinFlags_)) {
		ImGui::End();
		return;
	}

	PlayControl();

	ImGui::End();
}

void Console::Assets() {
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	if(!ImGui::Begin("Assets", nullptr, imguiWinFlags_)) {
		ImGui::End();
		return;
	}

	ImVec2 texSize = ImVec2(128, 128);
	auto texAll = TextureManager::GetInstance()->GetTextureAll();

	for(auto& tex : texAll) {
		ImGui::Image(
			ImTextureID(tex.second.GetGPUHandle().ptr), 
			texSize
		);
	}


	ImGui::End();
}

void Console::PlayControl() {

	ONE::WinApp* winApp = ONEngine::GetWinApps().at("Game").get();
	HWND hwnd           = winApp->GetHWND();

	if(ImGui::Button("Play")) {
		/// game window create
		if(!IsWindowVisible(hwnd)) {
			winApp->ShowGameWindow();
		}

	}

	if(ImGui::Button("Stop")) {
		/// game window desctory
		if(IsWindowVisible(hwnd)) {
			winApp->DestoryGameWindow();
		}

	}
	
}

void Console::CLI() {
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	if(!ImGui::Begin("Command Line Interface", nullptr, imguiWinFlags_)) {
		ImGui::End();
		return;
	}

	pCLI_->RenderCLI();


	ImGui::End();
}

void Console::DescriptorHeapsMunuBar() {
	if(ImGui::BeginMenu("descriptor heap")) {

		DxDescriptorHeap<HeapType::CBV_SRV_UAV>* pSRVHeap = ONEngine::GetDxCommon()->GetSRVDescriptorHeap();
		DxDescriptorHeap<HeapType::RTV>*         pRTVHeap = ONEngine::GetDxCommon()->GetRTVDescriptorHeap();
		DxDescriptorHeap<HeapType::DSV>*         pDSVHeap = ONEngine::GetDxCommon()->GetDSVDescriptorHeap();

		ImGui::Text(std::format("srv heap count : {}/{}", pSRVHeap->GetUsedIndexCount(), pSRVHeap->GetMaxHeapSize()).c_str());
		ImGui::Text(std::format("rtv heap count : {}/{}", pRTVHeap->GetUsedIndexCount(), pRTVHeap->GetMaxHeapSize()).c_str());
		ImGui::Text(std::format("dsv heap count : {}/{}", pDSVHeap->GetUsedIndexCount(), pDSVHeap->GetMaxHeapSize()).c_str());

		ImGui::EndMenu();
	}
}

void Console::WindowSettingMenuBar() {
	if(ImGui::BeginMenu("setting")) {

		/// ===================================================
		/// child window 
		/// ===================================================
		if(ImGui::BeginMenu("child window setting")) {

			bool noMove = imguiWinFlags_ & ImGuiWindowFlags_NoMove;
			if(ImGui::Checkbox("NoMove", &noMove)) {
				if(noMove) {
					imguiWinFlags_ |= ImGuiWindowFlags_NoMove;
				} else {
					imguiWinFlags_ &= ~ImGuiWindowFlags_NoMove;
				}
			}
			ImGui::EndMenu();
		}

		/// ===================================================
		/// parent window 
		/// ===================================================
		if(ImGui::BeginMenu("parent window setting")) {

			bool noResize = parentWinFlags_ & ImGuiWindowFlags_NoResize;
			if(ImGui::Checkbox("NoResize", &noResize)) {
				if(noResize) {
					parentWinFlags_ |= ImGuiWindowFlags_NoResize;
				} else {
					parentWinFlags_ &= ~ImGuiWindowFlags_NoResize;
				}
			}

			bool noMove = parentWinFlags_ & ImGuiWindowFlags_NoMove;
			if(ImGui::Checkbox("NoMove", &noMove)) {
				if(noMove) {
					parentWinFlags_ |= ImGuiWindowFlags_NoMove;
				} else {
					parentWinFlags_ &= ~ImGuiWindowFlags_NoMove;
				}
			}
			ImGui::EndMenu();
		}

		ImGui::EndMenu();
	}
}

void Console::VariableGroupArray() {
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	if(!ImGui::Begin("variable manager", nullptr, imguiWinFlags_)) {
		ImGui::End();
		return;
	}

	pVariableManager_->DebuggingGroupArray();

	ImGui::End();
}

void Console::CreateGameObjectMenuBar() {
	if(ImGui::BeginMenu("command")) {

		if(ImGui::Button("create game object")) {
			std::locale::global(std::locale(""));

			std::string inputText = ONE::ConvertString(ShowSaveFileDialog());
			if(!inputText.empty()) {
				pCLI_->ExecuteCommand("CreateGameObject", inputText);
			}
		}

		ImGui::EndMenu();
	}
}
