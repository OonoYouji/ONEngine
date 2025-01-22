#include "DemoObject.h"

/// std
#include <format>

/// externals
#include <imgui.h>

/// engine
#include "GraphicManager/ModelManager/ModelManager.h"
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "ComponentManager/Collider/SphereCollider.h"
#include "FrameManager/Time.h"


void DemoObject::Initialize() {

	/*model* sphere = modelmanager::load("sphere");

	meshrenderer* meshrenderer = addcomponent<meshrenderer>();
	meshrenderer->setmodel(sphere);

	spherecollider* spherecollider = addcomponent<spherecollider>(sphere);*/

}

void DemoObject::Update() {


}

void DemoObject::Debug() {

	ImGui::Text("fps: %f", 1.0f / Time::DeltaTime());


}

void DemoObject::OnCollisionEnter(BaseGameObject* _other) {

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
