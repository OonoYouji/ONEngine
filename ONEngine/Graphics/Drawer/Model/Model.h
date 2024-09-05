#pragma once

#include <vector>

#include <PipelineState.h>

#include <Vector2.h>
#include <Vector3.h>
#include <Vector4.h>
#include <Matrix4x4.h>

#include <Transform.h>

#include <Mesh.h>
#include <Material.h>


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
	void Draw(Transform* transform, FillMode fillMode = kSolid);
	void Draw(Transform* transform, Material* material, FillMode fillMode = kSolid);


	/// <summary>
	/// CommandListに必要な情報をセット
	/// </summary>
	void DrawCall(ID3D12GraphicsCommandList* commandList, Material* material = nullptr);

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

	
	/// <summary>
	/// メッシュの追加
	/// </summary>
	/// <param name="mesh"></param>
	void AddMesh(const Mesh& mesh);

	/// <summary>
	/// マテリアルの追加
	/// </summary>
	/// <param name="material"></param>
	void AddMaterial(const Material& material);


	const std::vector<Mesh>& GetMeshes() const { return meshes_; }


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	FillMode fillMode_;

	std::vector<Mesh> meshes_;
	std::vector<Material> materials_;


};