#pragma once
#include <Vector2.h>
#include <Vector3.h>
#include <Vector4.h>

#include <wrl/client.h>
#include <DxCommon.h>
#include <PipelineState.h>

using namespace Microsoft::WRL;


class LineDrawer2D final {
public:
	LineDrawer2D() = default;
	~LineDrawer2D() = default;
public:

	struct LineVertexDate {
		Vector4 postion;
		Vector4 color;
	};

public:
	static LineDrawer2D* GetInstance();

	void Initialize();

	void Finalize();

	void PreDraw();

	void PostDraw();

	void Draw(const Vector2& v1, const Vector2& v2, const Vector4& color);

private:

	void CreateVertexBuffer(size_t vertexCount);

private:

	ComPtr<ID3D12Resource> vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vbv_;

	std::vector<LineVertexDate> vertices_;
	std::unique_ptr<PipelineState> pipeline_ = nullptr;
	PipelineState::Shader shader_;

private:
	LineDrawer2D(const LineDrawer2D&) = delete;
	LineDrawer2D& operator= (const LineDrawer2D&) = delete;
	LineDrawer2D& operator= (LineDrawer2D&&) = delete;
};
