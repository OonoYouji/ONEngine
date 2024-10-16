#include "Ground.h"

/// engine
#include "GraphicManager/ModelManager/ModelManager.h"
#include "ImGuiManager/ImGuiManager.h"
#include "Input/Input.h"

/// components
#include <ComponentManager/MeshRenderer/MeshRenderer.h>
#include "Game/CustomComponents/EarthRenderer/EarthRenderer.h"

float Ground::groundScale_ = 11.0f
; void Ground::Initialize() {
	//auto meshRenderer = AddComponent<MeshRenderer>();
	//meshRenderer->SetModel("Sphere");
	//meshRenderer->SetMaterial("uvChecker");

	auto er = AddComponent<EarthRenderer>();
	er->SetEarthTransform(pTransform_);

	pTransform_->quaternion = { 0,0,0,1 };
	pTransform_->rotateOrder = QUATERNION;

	pTransform_->scale = { groundScale_,groundScale_,groundScale_ };
	UpdateMatrix();

}

void Ground::Update() {

	
}



void Ground::Debug() {

}

