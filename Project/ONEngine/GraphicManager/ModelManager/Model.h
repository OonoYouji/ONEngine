#pragma once

#include <vector>

#include "GraphicManager/PipelineState/PipelineState.h"

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix4x4.h"

#include "ComponentManager/Transform/Transform.h"

#include "GraphicManager/Drawer/Mesh/Mesh.h"
#include "GraphicManager/Drawer/Material/Material.h"


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
	void DrawCall(
		ID3D12GraphicsCommandList* commandList, Material* material = nullptr,
		uint32_t materialRootParamIndex = 2, uint32_t textureRootParamIndex = 4);

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
	std::vector<Mesh>& GetMeshes() { return meshes_; }

	const std::vector<Material>& GetMaterials() const { return materials_; }
	std::vector<Material>& GetMaterials() { return materials_; }


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	FillMode fillMode_;

	std::vector<Mesh> meshes_;
	std::vector<Material> materials_;

};