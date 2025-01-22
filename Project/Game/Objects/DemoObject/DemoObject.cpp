#include "DemoObject.h"

/// std
#include <format>

/// externals
#include <imgui.h>

/// engine
#include "GraphicManager/ModelManager/ModelManager.h"
#include "GraphicManager/TextureManager/TextureManager.h"
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "ComponentManager/Collider/SphereCollider.h"
#include "FrameManager/Time.h"
#include "GraphicManager/Drawer/LineDrawer/Line3D.h"


void DemoObject::Initialize() {

	Model* sphere = ModelManager::Load("Sphere");

	renderer_ = AddComponent<MeshRenderer>();
	renderer_->SetModel(sphere);
	renderer_->SetMaterial("white2x2.png");

	SphereCollider* sphereCollider = AddComponent<SphereCollider>(sphere);


	mass_ = 10.0f;
}

void DemoObject::Update() {

	pTransform_->position += velocity_ * Time::DeltaTime();

}

void DemoObject::Debug() {

	ImGui::DragFloat("mass", &mass_);
	ImGui::DragFloat3("velocity", &velocity_.x);

	Line3D::GetInstance()->Draw(
		GetPosition(),
		GetPosition() + velocity_.Normalize() * 3.0f
	);

}

std::pair<Vec3, Vec3> ComputeCollisionVelocities(float _mass1, const Vec3& _velocity1, float _mass2, const Vec3& _velocity2, float _coefficientRestitution, const Vec3& _normal) {

	/// 衝突面法線方向 (射影) とその他に分解する
	const Vec3&& project1 = Vec3::Project(_velocity1, _normal);
	const Vec3&& project2 = Vec3::Project(_velocity2, _normal);

	const Vec3&& sub1 = _velocity1 - project1;
	const Vec3&& sub2 = _velocity2 - project2;


	/// 衝突面方向に対する反発後の速度を求める
	const Vec3&& velocityAfter1 = project1 * (-_coefficientRestitution * _mass2) + project2 * ((1.0f + _coefficientRestitution) * _mass2) + sub1;
	const Vec3&& velocityAfter2 = project1 * ((1.0f + _coefficientRestitution) * _mass1) + project2 * (-_coefficientRestitution * _mass1) + sub2;


	return std::pair<Vec3, Vec3>(velocityAfter1 + sub1, velocityAfter2 + sub2);
}




void RedBall::Initialize() {
	DemoObject::Initialize();
	renderer_->SetColor({ 1, 0, 0, 1 });
}

void RedBall::Update() {
	pTransform_->position += velocity_ * Time::DeltaTime();
}

void RedBall::Debug() {
	DemoObject::Debug();
}

void RedBall::OnCollisionEnter(BaseGameObject* _other) {

	if (_other->GetTag() != "WhiteBall") {
		return;
	}

	DemoObject* demo2  = static_cast<DemoObject*>(_other);
	Vec3        normal = Vec3::Normalize(GetPosition() - demo2->GetPosition());

	/// 衝突後の速度計算
	std::pair<Vec3, Vec3> velocities = ComputeCollisionVelocities(
		mass_, velocity_,
		demo2->GetMass(), demo2->GetVelocity(),
		1.0f, normal
	);

	/// 適用
	velocity_ = velocities.first;
	demo2->SetVelocity(velocities.second);

}




void WhiteBall::Initialize() {
	DemoObject::Initialize();
	renderer_->SetColor({ 1, 1, 1, 1 });

	pTransform_->position = { -5, 0, 0 };
	velocity_ = { 0, 0, 0 };
}

void WhiteBall::Update() {
	pTransform_->position += velocity_ * Time::DeltaTime();
}

void WhiteBall::Debug() {
	DemoObject::Debug();
}

void WhiteBall::OnCollisionEnter(BaseGameObject* _other) {}
