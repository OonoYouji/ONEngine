#include "DemoObject.h"


/// std
#include <format>
#include <numbers>

/// engine
#include "SceneManager/SceneManager.h"
#include "Input/Input.h"
#include "CustomMath/ImGui/ImGuiGizmo.h"
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "Objects/Camera/Manager/CameraManager.h"


void DemoObject::Initialize() {
	renderer_ = AddComponent<MeshRenderer>();
	renderer_->SetModel("Sphere");
	renderer_->SetMaterial("monsterBall.png");

	pTransform_->rotate.y = std::numbers::pi_v<float> * -0.5f;
}

void DemoObject::Update() {


}

void DemoObject::Debug() {

	ImGui::DragFloat("shininess", &shininess, 0.1f, 0.0f, 100.0f);

	Vec4 color = renderer_->GetColor();
	if (ImGui::ColorEdit3("color", &color.x)) {
		renderer_->SetColor(color);
	}

}
