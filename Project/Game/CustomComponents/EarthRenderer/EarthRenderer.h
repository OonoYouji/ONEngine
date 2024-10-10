#pragma once

/// std
#include <memory>

/// engine
#include "GraphicManager/PipelineState/PipelineState.h"

/// math
#include "Math/Vector4.h"

/// base
#include "ComponentManager/Base/BaseComponent.h"





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

	static void SInitialize(ID3D12GraphicsCommandList* commandList, uint32_t maxEntityNum);
	static void SFinalize();
	
	static void PreDraw();
	static void PostDraw();

	
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
