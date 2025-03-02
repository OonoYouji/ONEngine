#pragma once

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Graphics/Pipelines/Primitive/Line2DRenderingPipeline.h"
#include "Engine/Core/Utility/Math/Vector2.h"


/// ===================================================
/// 2Dライン描画コンポーネント
/// ===================================================
class Line2DRenderer final : public IRenderComponent {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================
	
	Line2DRenderer();
	~Line2DRenderer();

	/// @brief lineの設定
	/// @param _start 初期値
	/// @param _end   終了値
	/// @param _color 色
	void SetLine(const Vec2& _start, const Vec2& _end, const Vec4& _color);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	size_t refCount_;
	Line2DRenderingPipeline::RenderingData renderingData_;


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================
	
	const Line2DRenderingPipeline::RenderingData& GetRenderingData() const { return renderingData_; }
	Line2DRenderingPipeline::RenderingData* GetRenderingDataPtr() { return &renderingData_; }

};

