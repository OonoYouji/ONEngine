#pragma once

/// std
#include <memory>

/// engine
#include "GraphicManager/GraphicsEngine/DirectX12/DxDescriptor.h"
#include "GraphicManager/PipelineState/PipelineState.h"

/// math
#include "Math/Vector4.h"

/// components
#include "ComponentManager/Base/BaseComponent.h"
#include "ComponentManager/Transform/Transform.h"


/// 前方宣言
class DirectionalLight;


/// ===================================================
/// 地球を描画する用のコンポーネント
/// ===================================================
class EarthRenderer final : public BaseComponent {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	EarthRenderer();
	~EarthRenderer();
	
	
	/// ===================================================
	/// public : static methods
	/// ===================================================

	static void SInitialize(
		ID3D12GraphicsCommandList* commandList,
		ONE::DxDescriptor* dxDescriptor,
		uint32_t maxEntityNum
	);
	static void SFinalize();
	
	static void PreDraw();
	static void PostDraw();

	static void SetEarthTransform(Transform* _transform);
	static void SetDirectionalLight(DirectionalLight* _directionalLight);

	
	/// ===================================================
	/// public : methods
	/// ===================================================

	void Initialize() override;
	void Update()     override;
	void Draw()       override;


	/// <summary>
	/// 表示する半径の描画
	/// </summary>
	/// <param name="_radius"></param>
	void SetRadius(float _radius);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	Vec4  position_;
	float radius_;

};
