#include "DemoObject.h"


/// std
#include <format>

#include "SceneManager/SceneManager.h"
#include "Input/Input.h"
#include "CustomMath/ImGui/ImGuiGizmo.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "Objects/Camera/Manager/CameraManager.h"


void DemoObject::Initialize() {
	renderer_ = AddComponent<MeshRenderer>();
	renderer_->SetModel("Sphere");
	renderer_->SetMaterial("uvChecker.png");
}

void DemoObject::Update() {


}

void DemoObject::Debug() {

	

}
