#include "DeadEffect.h"


#include "ComponentManager/ParticleSystem/ParticleSystem.h"


DeadEffect::DeadEffect() {
	CreateTag(this);
}

DeadEffect::~DeadEffect() {}

void DeadEffect::Initialize() {
	particleSyste_ = AddComponent<ParticleSystem>(12, "Sphere");
}

void DeadEffect::Update() {

}
