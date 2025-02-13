#include "Line3DRenderer.h"

Line3DRenderer::Line3DRenderer() {}
Line3DRenderer::~Line3DRenderer() {}


void Line3DRenderer::SetLine(const Vector3& _start, const Vector3& _end, const Vector4& _color) {
	
	///< 今回追加するラインの頂点数が、現在の頂点数を超える場合は、頂点数を増やす
	if (vertices_.size() < refCount_ * 2 + 2) {
		vertices_.resize(refCount_ * 2 + 2);
	}

	vertices_[refCount_ * 2 + 0].position = { _start.x, _start.y, 0.0f, 1.0f };
	vertices_[refCount_ * 2 + 1].position = { _end.x,   _end.y,   0.0f, 1.0f };

	vertices_[refCount_ * 2 + 0].color = _color;
	vertices_[refCount_ * 2 + 1].color = _color;

	refCount_++;
}
