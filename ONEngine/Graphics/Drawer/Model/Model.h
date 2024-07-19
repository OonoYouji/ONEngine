#pragma once

#include <PipelineState.h>

#include <Vector4.h>

struct VertexData {
	Vec4 pos;
};



/// ===================================================
/// モデル
/// ===================================================
class Model final {
public:

	Model();
	~Model();

public:
	
	/// ===================================================
	/// public : methods
	/// ===================================================


	void Initialize();

	void Draw();

	/// <summary>
	/// FillModeのセット
	/// </summary>
	/// <param name="fillMode"></param>
	void SetFillMode(FillMode fillMode);

	/// <summary>
	/// FillModeのゲッタ
	/// </summary>
	/// <returns></returns>
	inline FillMode GetFillMode() const { return fillMode_; }


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	FillMode fillMode_;

	ComPtr<ID3D12Resource> vertexBuffer_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vbv_;


};