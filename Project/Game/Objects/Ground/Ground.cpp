#include "Ground.h"

/// engine
#include "GraphicManager/ModelManager/ModelManager.h"
#include "ImGuiManager/ImGuiManager.h"
#include "Input/Input.h"

/// components
#include <ComponentManager/MeshRenderer/MeshRenderer.h>



void Ground::Initialize() {
	auto meshRenderer = AddComponent<MeshRenderer>();
	pTransform_->rotateOrder = QUATERNION;
	pTransform_->quaternion = { 0,0,0,1 };
	meshRenderer->SetModel("Sphere");
	meshRenderer->SetMaterial("uvChecker");

	//auto particle = AddComponent<ParticleSystem>(12, "Sphere");
	//AddComponent<SphereCollider>(ModelManager::Load("Sphere"));
	pTransform_->scale = { 11,11,11 };
	/*SetPositionZ(10.0f);*/
	UpdateMatrix();
}

void Ground::Update() {

	
}



void Ground::Debug() {

}

