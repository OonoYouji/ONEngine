#define NOMINMAX
#include "Console.h"


#include <GameObjectManager.h>
#include <SceneManager.h>
#include <TextureManager.h>


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
	pSceneManager_ = SceneManager::GetInstance();


}

void Console::Update() {
#ifdef _DEBUG
	ParentWindow();

	Inspector();
	Herarchy();

	Scene();
	Debug();

	Assets();
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




	ImGui::End();
}

void Console::Herarchy() {
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	if(!ImGui::Begin("Heararchy", nullptr, imguiWinFlags_)) {
		ImGui::End();
		return;
	}


	ImGui::End();
}

void Console::Scene() {
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	if(!ImGui::Begin("Scene", nullptr, imguiWinFlags_)) {
		ImGui::End();
		return;
	}

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
	texPos.y = std::max(texPos.y, min.y);

	/// scene gpu handle ptr
	auto renderTex = pSceneManager_->GetSceneLayer(0)->GetRenderTexture();
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


	ImGui::End();
}

void Console::Assets() {
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	if(!ImGui::Begin("Assets", nullptr, imguiWinFlags_)) {
		ImGui::End();
		return;
	}


	ImGui::End();
}
