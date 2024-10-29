#pragma once

/// std
#include <functional>

/// lib
#include "Math/Vector3.h"

class Particle;

class ParticleField {
public:

	ParticleField() {}
	~ParticleField() {}

	void Initialize();
	void Update(Particle* _particle);

	/// <summary>
	/// fieldに入っているparticleの処理を行う関数
	/// </summary>
	/// <param name="_updateFunction"></param>
	void SetUpdateFunction(const std::function<void(Particle*)>& _updateFunction);

	const Vec3& GetMin() const { return minPosition_; }
	const Vec3& GetMax() const { return maxPosition_; }

	void SetMin(const Vec3& _min);
	void SetMax(const Vec3& _max);

private:

	std::function<void(Particle*)> updateFunction_;

	Vec3 minPosition_, maxPosition_; /// fieldの最小と最大座標

};