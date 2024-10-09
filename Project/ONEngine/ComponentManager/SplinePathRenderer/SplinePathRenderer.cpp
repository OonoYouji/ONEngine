#include "SplinePathRenderer.h"

#include <algorithm>
#include <cassert>

#include "GraphicManager/Drawer/LineDrawer/Line3D.h"


SplinePathRenderer::SplinePathRenderer(uint32_t segmentDivisions) {
	segmentDivisions_ = segmentDivisions;
}


void SplinePathRenderer::Initialize() {
	line3D_ = Line3D::GetInstance();


}

void SplinePathRenderer::Update() {

	segmentPoints_.clear();

	const size_t segmentCount = anchorPointArray_.size() * segmentDivisions_;
	for(size_t i = 0; i < segmentCount + 1; ++i) {
		float t = 1.0f / segmentCount * i;
		Vec3 position = CaclationSpline(anchorPointArray_, t);
		segmentPoints_.push_back(position);
	}
}

void SplinePathRenderer::Draw() {

	for(auto itr = segmentPoints_.begin(); itr != segmentPoints_.end(); ++itr) {
		auto nextItr = std::next(itr);

		/// nextItrが最後の要素でなければ描画する
		if(nextItr != segmentPoints_.end()) {
			line3D_->Draw(*itr, *nextItr, Vec4::kRed, Vec4::kRed);
		}
	}

}



void SplinePathRenderer::AddAnchorPoint(const Vec3& anchorPoint) {
	anchorPointArray_.push_back(anchorPoint);
}

void SplinePathRenderer::SetAnchorPointArray(const std::vector<Vec3>& anchorPointArray) {
	anchorPointArray_ = anchorPointArray;
}

Vec3 SplinePathRenderer::CaclationSpline(const std::vector<Vec3>& vertices, float t) {
	assert(vertices.size() >= 4 && "制御点は4点以上必要です");

	size_t division = vertices.size() - 1;
	float areaWidth = 1.0f / static_cast<float>(division);

	///- 区間内の始点を0.0f, 終点を1.0f としたときの現在位置
	float t_2 = std::fmod(t, areaWidth) * division;
	t_2 = std::clamp(t_2, 0.0f, 1.0f);

	///- 区画番号
	size_t index = static_cast<size_t>(t / areaWidth);
	if(index > vertices.size() - 2) {
		index = vertices.size() - 2;
	}

	size_t indices[4]{
		index - 1,
		index,
		index + 1,
		index + 2
	};

	if(index == 0) {
		indices[0] = indices[1];
	}

	if(indices[3] >= vertices.size()) {
		indices[3] = indices[2];
	}

	const Vec3& p0 = vertices[indices[0]];
	const Vec3& p1 = vertices[indices[1]];
	const Vec3& p2 = vertices[indices[2]];
	const Vec3& p3 = vertices[indices[3]];

	return SplineInterpolation({ p0, p1, p2, p3 }, t_2);
}

Vec3 SplinePathRenderer::SplineInterpolation(const std::array<Vec3, 4>& vs, float t) {
	const float s = 0.5f;

	float t2 = t * t; // t^2
	float t3 = t2 * t; // t^3

	Vec3 e3 = -vs[0] + (vs[1] * 3.0f) - (vs[2] * 3.0f) + vs[3];
	Vec3 e2 = (vs[0] * 2.0f) - (vs[1] * 5.0f) + (vs[2] * 4.0f) - vs[3];
	Vec3 e1 = -vs[0] + vs[2];
	Vec3 e0 = vs[1] * 2.0f;

	return ((e3 * t3) + (e2 * t2) + (e1 * t) + e0) * s;
}
