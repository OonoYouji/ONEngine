#include "Line3DRenderer.h"

using namespace ONEngine;

Line3DRenderer::Line3DRenderer() {
	lineCount_ = 0;
}
Line3DRenderer::~Line3DRenderer() {}


void Line3DRenderer::SetLine(const Vector3& _start, const Vector3& _end, const Vector4& _color) {
	
	///< 今回追加するラインの頂点数が、現在の頂点数を超える場合は、頂点数を増やす
	if (vertices_.size() < lineCount_ * 2 + 2) {
		vertices_.resize(lineCount_ * 2 + 2);
	}

	vertices_[lineCount_ * 2 + 0].position = { _start.x, _start.y, _start.z, 1.0f };
	vertices_[lineCount_ * 2 + 1].position = { _end.x,   _end.y,   _end.z,   1.0f };

	vertices_[lineCount_ * 2 + 0].color = _color;
	vertices_[lineCount_ * 2 + 1].color = _color;

	lineCount_++;
}
