#include "Tornado.h"

#include <ImGuiManager.h>

#include <Component/MeshRenderer/MeshRenderer.h>


void Tornado::Initialize() {

	/// 親となるtransformの初期化
	pivot_.Initialize();
	pivot_.rotateOrder = QUATERNION;

	SetParent(&pivot_);

	/// 中心からのローカル座標を計算、初期化する

	/// mesh renderer 初期化
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel("Sphere");

}

void Tornado::Update() {}

void Tornado::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {
		
		if(ImGui::TreeNodeEx("pivot", ImGuiTreeNodeFlags_DefaultOpen)) {
			pivot_.Debug();
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
}
