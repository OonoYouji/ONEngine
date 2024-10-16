#pragma once

/// std
#include <cstdint>

/// graphic
#include "GraphicManager/ModelManager/Model.h"

/// base class
#include "ComponentManager/Base/BaseComponent.h"

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

	static void PreDraw();
	static void PostDraw();


	/// ===================================================
	/// public : overriding methods
	/// ===================================================

	void Initialize() override;
	void Update()     override;
	void Draw()       override;
	void Debug()      override;

private:

	const uint32_t kMaxInstanceCount_ = 0u;
	Model* model_ = nullptr;

};