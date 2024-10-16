#pragma once

/// std
#include <cstdint>

/// graphic
#include "GraphicManager/ModelManager/Model.h"

/// base class
#include "ComponentManager/Base/BaseComponent.h"
#include <GraphicManager/Light/DirectionalLight.h>


struct RenderElement {
	Transform* pTransform = nullptr;
};


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

	static void SetDirectionalLight(class DirectionalLight* _directionalLight);


	/// ===================================================
	/// public : overriding methods
	/// ===================================================

	void Initialize() override;
	void Update()     override;
	void Draw()       override;
	void Debug()      override;

private:

	const uint32_t kMaxInstanceCount_ = 0u;
	std::vector<RenderElement> renderElementArray_;
	Model* model_ = nullptr;

	/// buffer, 
	Microsoft::WRL::ComPtr<ID3D12Resource> transformArrayBuffer_ = nullptr;
	Mat4*                                  mappingRenderElement_ = nullptr;
	D3D12_GPU_DESCRIPTOR_HANDLE            gpuHandle_;
	D3D12_CPU_DESCRIPTOR_HANDLE            cpuHandle_;

};