#include "Tornado.h"

#include <ImGuiManager.h>

#include <Component/MeshRenderer/MeshRenderer.h>


void Tornado::Initialize() {

	/// mesh renderer 初期化
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel("Sphere");

}

void Tornado::Update() {}

void Tornado::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {
		
		if(ImGui::TreeNodeEx("player", ImGuiTreeNodeFlags_DefaultOpen)) {

			void* pointer = reinterpret_cast<void*>(pPlayer_);
			ImGui::DragInt("address", reinterpret_cast<int*>(&pointer));

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
}



void Tornado::SetPlayer(Player* _player) {
	pPlayer_ = _player;
}
