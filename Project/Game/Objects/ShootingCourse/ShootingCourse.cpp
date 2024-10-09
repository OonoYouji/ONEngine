#include "ShootingCourse.h"

#include "ComponentManager/SplinePathRenderer/SplinePathRenderer.h"

ShootingCourse::ShootingCourse() {
	CreateTag(this);
}

ShootingCourse::~ShootingCourse() {}

void ShootingCourse::Initialize() {
	SplinePathRenderer* spr = AddComponent<SplinePathRenderer>(6);
	spr->AddAnchorPoint({ 0,0,0 });
	spr->AddAnchorPoint({ 0,0,1 });
	spr->AddAnchorPoint({ 0,1,2 });
	spr->AddAnchorPoint({ 1,1,3 });
}

void ShootingCourse::Update() {

}
