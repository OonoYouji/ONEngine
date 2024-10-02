#pragma once

#include <cmath>
#include <list>

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

private:

	uint32_t        segmentDivisions_ = 12u;
	std::list<Vec3> anchorPointArray_;

};