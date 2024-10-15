#pragma once

/// std
#include <functional>

class Particle;

class ParticleField {
public:

	ParticleField() {}
	~ParticleField() {}

	void Initialize();
	void Update();

private:

	std::function<void(Particle*)> updateFunction_;

};