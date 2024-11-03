#include "ParticleField.h"

/// engine
#include "FrameManager/Time.h"

#include "ParticleSystem.h"


void ParticleField::Initialize() {

	SetUpdateFunction([](Particle* particle) {
		Transform* transform = particle->GetTransform();
		transform->position += Vec3(0, 1, 0) * Time::DeltaTime();
	});

	SetMin(-Vec3::kOne);
	SetMax(+Vec3::kOne);

}

void ParticleField::Update(Particle* _particle) {
	updateFunction_(_particle);
}

void ParticleField::SetUpdateFunction(const std::function<void(Particle*)>& _updateFunction) {
	updateFunction_ = _updateFunction;
}

void ParticleField::SetMin(const Vec3& _min) {
	minPosition_ = _min;
}

void ParticleField::SetMax(const Vec3& _max) {
	maxPosition_ = _max;
}
