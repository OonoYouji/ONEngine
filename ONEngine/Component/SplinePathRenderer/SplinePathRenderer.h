#pragma once

#include <cmath>
#include <list>
#include <array>

#include <Vector3.h>

#include <Component/Base/BaseComponent.h>

class SplinePathRenderer final : public BaseComponent {
public:

	SplinePathRenderer(uint32_t segmentDivisions);
	~SplinePathRenderer() {}

	void Initialize() override;
	void Update()     override;
	void Draw()       override;

	void AddAnchorPoint(const Vec3& anchorPoint);

	Vec3 CaclationSpline(const std::vector<Vec3>& vertices, float t);

	Vec3 SplineInterpolation(const std::array<Vec3, 4>& vertices, float t);

private:

	uint32_t          segmentDivisions_ = 12u;
	std::vector<Vec3> anchorPointArray_;
	std::vector<Vec3> segmentPoints_;

	class Line3D*   line3D_           = nullptr;

};