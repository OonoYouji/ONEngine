#pragma once

/// std
#include <cstdint>

/// graphic
#include "GraphicManager/ModelManager/Model.h"
#include "GraphicManager/Drawer/Material/Material.h"

/// base class
#include "ComponentManager/Base/BaseComponent.h"
#include <GraphicManager/Light/DirectionalLight.h>



/// ===================================================
/// メッシュの描画をインスタンシング描画で行うクラス
/// ===================================================
class MeshInstancingRenderer final : public BaseComponent {
public:

	MeshInstancingRenderer(uint32_t maxInstanceCount);
	~MeshInstancingRenderer() {}


	/// ===================================================
	/// public : static methods
	/// ===================================================
	
	static void SInitialize(ID3D12GraphicsCommandList* _commandList);
	static void SFinalize();

	static void SetLightGroup(class LightGroup* _lightGroup);

	static void PreDraw();
	static void PostDraw();


	/// ===================================================
	/// public : overriding methods
	/// ===================================================

	void Initialize() override;
	void Update()     override;
	void Draw()       override;
	void Debug()      override;

	void DrawCall();


	/// ===================================================
	/// public : not overriding methods
	/// ===================================================

	/// <summary>
	/// transformの追加
	/// </summary>
	/// <param name="transform"> : model を描画したい座標 </param>
	void AddTransform(Transform* transform);

	/// <summary>
	/// transform arrayの設定
	/// </summary>
	/// <param name="transformArray"> : model を描画したいすべての座標 </param>
	void SetTransformArray(const std::vector<Transform*>& transformArray);

	/// <summary>
	/// transform arrayのリセット
	/// </summary>
	void ResetTransformArray();

private:
	
	/// ===================================================
	/// private : objects
	/// ===================================================

	const uint32_t                         kMaxInstanceCount_    = 0u;
	std::vector<Transform*>                transformArray_;
	Model*                                 model_                = nullptr;
	std::unique_ptr<Material>              material_             = nullptr;

	/// buffer
	Microsoft::WRL::ComPtr<ID3D12Resource> transformArrayBuffer_ = nullptr;
	Transform::BufferData*                 mappingData_          = nullptr;
	D3D12_GPU_DESCRIPTOR_HANDLE            gpuHandle_;
	D3D12_CPU_DESCRIPTOR_HANDLE            cpuHandle_;


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	void SetModel(const std::string& filePath);
	void SetModel(Model* model);

	/// @brief マテリアルの生成と色の設定
	/// @param _color 
	void CreateMaterial(const std::string& _textureName);

	/// @brief マテリアルのゲット
	/// @return 
	Material* GetMaterial() { return material_.get(); }


};