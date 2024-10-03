#include "Ground.h"

#include <ModelManager.h>

#include <Input.h>
#include <Component/MeshRenderer/MeshRenderer.h>
#include <Component/SpriteRenderer/SpriteRenderer.h>
#include <Particle/ParticleSystem.h>
#include <Component/Collider/SphereCollider.h>
#include <Component/SplinePathRenderer/SplinePathRenderer.h>

#include <ImGuiManager.h>


void Ground::Initialize() {
	auto meshRenderer = AddComponent<MeshRenderer>();
	pTranform_->rotateOrder = QUATERNION;
	pTranform_->quaternion = { 0,0,0,1 };
	meshRenderer->SetModel("Sphere");
	meshRenderer->SetMaterial("uvChecker");

	//auto particle = AddComponent<ParticleSystem>(12, "Sphere");
	//AddComponent<SphereCollider>(ModelManager::Load("Sphere"));
	pTranform_->scale = { 11,11,11 };
	/*SetPositionZ(10.0f);*/
	UpdateMatrix();
}

void Ground::Update() {

	
}



void Ground::Debug() {

}

