#include "ShootingCourse.h"


void ShootingCourse::Initialize() {
	splinePathRenderer_ = AddComponent<SplinePathRenderer>(6);
}

void ShootingCourse::Update() {}
