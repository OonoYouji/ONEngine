#include "ParticleField.h"

/// engine
#include "FrameManager/Time.h"

#include "ParticleSystem.h"


void ParticleField::Initialize() {

	SetUpdateFunction([](Particle* particle) {
		Transform* transform = particle->GetTransform();
		transform->position += Vec3(0, 1, 0) * Time::DeltaTime();
	});

}

void ParticleField::Update() {

}

void ParticleField::SetUpdateFunction(const std::function<void(Particle*)>& _updateFunction) {
	updateFunction_ = _updateFunction;
}
