#include "ConsoleManager.h"

/// std
#include <format>

/// engine
#include "Core/ONEngine.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDescriptorHeap.h"

using namespace ONE;



void ConsoleManager::Initialize() {
	parentImGuiWindowFlags_ = 0;
	parentImGuiWindowFlags_ |= ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar;
	parentImGuiWindowFlags_ |= ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse;
	parentImGuiWindowFlags_ |= ImGuiWindowFlags_::ImGuiWindowFlags_NoMove;
	parentImGuiWindowFlags_ |= ImGuiWindowFlags_::ImGuiWindowFlags_NoResize;
	parentImGuiWindowFlags_ |= ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar;

	imguiWindowFlags_ = 0;
	imguiWindowFlags_ |= ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse;
	imguiWindowFlags_ |= ImGuiWindowFlags_::ImGuiWindowFlags_NoMove;


	RegisterMenuFunction([this]() { ChildWindowSetting(); });
	RegisterMenuFunction([this]() { DescriptorHeapUsedCount(); });

}

void ConsoleManager::Finalize() {}

void ConsoleManager::Update() {

	ParentWindow();

	for(auto& func : debugFunctions_) {
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		func(imguiWindowFlags_);
	}
}



void ConsoleManager::ParentWindow() {
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	if(!ImGui::Begin("Parent Window", nullptr, parentImGuiWindowFlags_)) {
		ImGui::End();
		return;
	}


	/// ===================================================
	/// main window menu baer
	/// ===================================================
	if(ImGui::BeginMenuBar()) {

		for(auto& func : menuFunctions_) {
			func();
		}

		ImGui::EndMenuBar();
	}

	ImGui::DockSpace(ImGui::GetID("DockSpace"));

	if(parentImGuiWindowFlags_ & ImGuiWindowFlags_NoMove) {
		ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
	}

	if(parentImGuiWindowFlags_ & ImGuiWindowFlags_NoResize) {
		ImGui::SetWindowSize(ImVec2(1280.0f, 720.0f));
	}


	ImGui::End();
}


void ConsoleManager::ChildWindowSetting() {
	if(ImGui::BeginMenu("setting")) {

		/// ===================================================
		/// child window 
		/// ===================================================
		if(ImGui::BeginMenu("child window setting")) {

			bool noMove = imguiWindowFlags_ & ImGuiWindowFlags_NoMove;
			if(ImGui::Checkbox("NoMove", &noMove)) {
				if(noMove) {
					imguiWindowFlags_ |= ImGuiWindowFlags_NoMove;
				} else {
					imguiWindowFlags_ &= ~ImGuiWindowFlags_NoMove;
				}
			}
			ImGui::EndMenu();
		}

		/// ===================================================
		/// parent window 
		/// ===================================================
		if(ImGui::BeginMenu("parent window setting")) {

			bool noResize = parentImGuiWindowFlags_ & ImGuiWindowFlags_NoResize;
			if(ImGui::Checkbox("NoResize", &noResize)) {
				if(noResize) {
					parentImGuiWindowFlags_ |= ImGuiWindowFlags_NoResize;
				} else {
					parentImGuiWindowFlags_ &= ~ImGuiWindowFlags_NoResize;
				}
			}

			bool noMove = parentImGuiWindowFlags_ & ImGuiWindowFlags_NoMove;
			if(ImGui::Checkbox("NoMove", &noMove)) {
				if(noMove) {
					parentImGuiWindowFlags_ |= ImGuiWindowFlags_NoMove;
				} else {
					parentImGuiWindowFlags_ &= ~ImGuiWindowFlags_NoMove;
				}
			}
			ImGui::EndMenu();
		}

		ImGui::EndMenu();
	}
}


void ConsoleManager::DescriptorHeapUsedCount() {
	if(ImGui::BeginMenu("descriptor heap")) {
		DxDescriptorHeap<HeapType::CBV_SRV_UAV>* pSRVHeap = ONEngine::GetDxCommon()->GetSRVDescriptorHeap();
		DxDescriptorHeap<HeapType::RTV>* pRTVHeap = ONEngine::GetDxCommon()->GetRTVDescriptorHeap();
		DxDescriptorHeap<HeapType::DSV>* pDSVHeap = ONEngine::GetDxCommon()->GetDSVDescriptorHeap();

		ImGui::Text(std::format("srv heap count : {}/{}", pSRVHeap->GetUsedIndexCount(), pSRVHeap->GetMaxHeapSize()).c_str());
		ImGui::Text(std::format("rtv heap count : {}/{}", pRTVHeap->GetUsedIndexCount(), pRTVHeap->GetMaxHeapSize()).c_str());
		ImGui::Text(std::format("dsv heap count : {}/{}", pDSVHeap->GetUsedIndexCount(), pDSVHeap->GetMaxHeapSize()).c_str());

		ImGui::EndMenu();
	}
}

void ConsoleManager::RegisterFunction(DebugFunction _function) {
	debugFunctions_.push_back(_function);
}

void ConsoleManager::RegisterMenuFunction(MenuFunction _menuFunction) {
	menuFunctions_.push_back(_menuFunction);
}
