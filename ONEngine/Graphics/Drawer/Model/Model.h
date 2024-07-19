#pragma once

#include <PipelineState.h>

#include <Vector2.h>
#include <Vector4.h>
#include <Matrix4x4.h>

#include <Transform.h>

struct VertexData {
	Vec4 pos;
	//Vec2 texcoord;
};

struct TransformData {
	Mat4 matWorld;
};

struct ViewProjectionData {
	Mat4 matVp;
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

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 描画
	/// </summary>
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

	Transform transform_;

	FillMode fillMode_;

	ComPtr<ID3D12Resource> vertexBuffer_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vbv_;

	ComPtr<ID3D12Resource> transformBuffer_ = nullptr;
	TransformData* transformData_ = nullptr;

	ComPtr<ID3D12Resource> viewProjectionBuffer_ = nullptr;
	ViewProjectionData* viewProjectionData_ = nullptr;


};