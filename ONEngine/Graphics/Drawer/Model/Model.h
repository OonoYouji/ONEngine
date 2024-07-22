#pragma once

#include <vector>

#include <PipelineState.h>

#include <Vector2.h>
#include <Vector4.h>
#include <Matrix4x4.h>

#include <Transform.h>

#include <Mesh.h>
#include <Material.h>


struct VertexData {
	Vec4 pos;
	Vec2 texcoord;
};

struct TransformData {
	Mat4 matWorld;
};

struct ViewProjectionData {
	Mat4 matVp;
};

struct MaterialData {
	Vec4 color;
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
	/// CommandListに必要な情報をセット
	/// </summary>
	void DrawCall(ID3D12GraphicsCommandList* commandList);

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



	void AddMesh(const Mesh& mesh);

	void AddMaterial(const Material& material);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	Transform transform_;

	FillMode fillMode_;

	ComPtr<ID3D12Resource> transformBuffer_ = nullptr;
	TransformData* transformData_ = nullptr;
	
	std::vector<Mesh> meshes_;
	std::vector<Material> materials_;

};