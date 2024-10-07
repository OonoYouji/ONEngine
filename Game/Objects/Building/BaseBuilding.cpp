#include "BaseBuilding.h"

#include <ModelManager.h>

#include <Input.h>
#include <Component/MeshRenderer/MeshRenderer.h>
#include <Component/SpriteRenderer/SpriteRenderer.h>
#include <Particle/ParticleSystem.h>
#include <Component/Collider/SphereCollider.h>
#include <Component/SplinePathRenderer/SplinePathRenderer.h>

#include <ImGuiManager.h>


void BaseBuilding::Initialize() {

	auto mesh = AddComponent<MeshRenderer>();
	mesh->SetModel("TestObject");

	////////////////////////////////////////////////////////////////////////////////////////////
	//  初期化
	////////////////////////////////////////////////////////////////////////////////////////////
	pTranform_->Initialize();
	pivot_.Initialize();
	////////////////////////////////////////////////////////////////////////////////////////////
	//  値セット
	////////////////////////////////////////////////////////////////////////////////////////////
	pivot_.quaternion = { 0,0,0,1 };
	pTranform_->position = { 0,0,-14 };
	pTranform_->rotate = { -1.5f,0,0 };
	////////////////////////////////////////////////////////////////////////////////////////////
    //  ペアレント
    ////////////////////////////////////////////////////////////////////////////////////////////
	/*pTranform_->rotateOrder = QUATERNION;*/
	pivot_.rotateOrder = QUATERNION;

	////ペアレント
	pTranform_->SetParent(&pivot_);

	pivot_.UpdateMatrix();
	UpdateMatrix();
}

void BaseBuilding::Update() {

	
	pivot_.UpdateMatrix();

}

void BaseBuilding::Debug() {

}


Quaternion BaseBuilding::MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle) {
	//    W A   ɕϊ 
	float halfAngle = angle * 0.5f;
	float sinHalfAngle = sin(halfAngle);

	//    K     ꂽ   x N g  
	Vector3 normalizedAxis = axis.Normalize();

	//  N H [ ^ j I   ̐     v Z
	float w = cos(halfAngle);
	float x = normalizedAxis.x * sinHalfAngle;
	float y = normalizedAxis.y * sinHalfAngle;
	float z = normalizedAxis.z * sinHalfAngle;

	// Vector4  Ƃ  ĕԂ 
	return Quaternion(x, y, z, w);
}