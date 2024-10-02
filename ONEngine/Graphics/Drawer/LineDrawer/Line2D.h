#pragma once
#include <Vector2.h>
#include <Vector3.h>
#include <Vector4.h>

#include <wrl/client.h>
#include <DxCommon.h>
#include <PipelineState.h>

using namespace Microsoft::WRL;


class Line2D final {
public:
	Line2D() = default;
	~Line2D() = default;
public:

	struct LineVertexDate {
		Vector4 postion;
		Vector4 color;
	};

public:
	static Line2D* GetInstance();

	void Initialize();

	void Finalize();

	void PreDraw();

	void PostDraw();

	void Draw(const Vector2& v1, const Vector2& v2, const Vector4& color);

private:

	void CreateVertexBuffer(size_t vertexCount);

private:

	static const int kMaxInstanceCount_;

	ComPtr<ID3D12Resource> vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vbv_;

	std::vector<LineVertexDate> vertices_;
	std::unique_ptr<PipelineState> pipeline_ = nullptr;
	PipelineState::Shader shader_;

private:
	Line2D(const Line2D&) = delete;
	Line2D& operator= (const Line2D&) = delete;
	Line2D& operator= (Line2D&&) = delete;
};
