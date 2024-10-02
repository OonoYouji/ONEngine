#include "SplinePathRenderer.h"


SplinePathRenderer::SplinePathRenderer(uint32_t segmentDivisions) {
	segmentDivisions_ = segmentDivisions;
}


void SplinePathRenderer::Initialize() {}

void SplinePathRenderer::Update() {}

void SplinePathRenderer::Draw() {}



void SplinePathRenderer::AddAnchorPoint(const Vec3& anchorPoint) {
	anchorPointArray_.push_back(anchorPoint);
}
