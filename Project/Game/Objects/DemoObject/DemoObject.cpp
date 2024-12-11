#include "DemoObject.h"

/// std
#include <format>

/// externals
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <ImGuizmo.h>

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"


void DemoObject::Initialize() {
	renderer_ = AddComponent<MeshRenderer>();
	renderer_->SetModel("Sphere");
	renderer_->SetMaterial("uvChecker.png");
}

void DemoObject::Update() {
	

}

void DemoObject::Debug() {

	ImVec2 viewportPos = ImGui::GetWindowPos();
	ImVec2 viewportSize = ImGui::GetWindowSize();

	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(viewportPos.x, viewportPos.y, viewportSize.x, viewportSize.y);


	if(ImGui::TreeNodeEx("Debug", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGuizmo::Manipulate(
			nullptr, // 視点行列（使用しない場合はnullptr）
			nullptr, // プロジェクション行列（使用しない場合はnullptr）
			ImGuizmo::OPERATION::TRANSLATE, // 操作タイプ（移動、回転、スケーリングなど）
			ImGuizmo::MODE::WORLD, // 操作モード（ローカルまたはワールド）
			&pTransform_->matTransform.m[0][0]  // 操作対象の行列
		);


		ImGui::TreePop();
	}
}

