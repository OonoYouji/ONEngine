#include "Line2DRenderer.h"

/// engine
#include "Engine/Graphics/Pipelines/Collection/RenderingPipelineCollection.h"

Line2DRenderer::Line2DRenderer() {
	renderingData_.vertices.reserve(32);
	refCount_ = 0;
}
Line2DRenderer::~Line2DRenderer() {}


void Line2DRenderer::SetLine(const Vec2& _start, const Vec2& _end, const Vec4& _color) {

	///< 今回追加するラインの頂点数が、現在の頂点数を超える場合は、頂点数を増やす
	if (renderingData_.vertices.size() < refCount_ * 2 + 2) {
		renderingData_.vertices.resize(refCount_ * 2 + 2);
	}

	renderingData_.vertices[refCount_ * 2 + 0].position = { _start.x, _start.y, 0.0f, 1.0f };
	renderingData_.vertices[refCount_ * 2 + 1].position = { _end.x,   _end.y,   0.0f, 1.0f };

	renderingData_.vertices[refCount_ * 2 + 0].color = _color;
	renderingData_.vertices[refCount_ * 2 + 1].color = _color;

	refCount_++;
}
