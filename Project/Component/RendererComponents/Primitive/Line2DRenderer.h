#pragma once

/// engine
#include "../../IComponent.h"
#include "Engine/Graphics/Pipelines/Primitive/Line2DRenderingPipeline.h"
#include "Engine/Utility/Math/Vector2.h"


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

	/// @brief rendering pipelineに描画データを追加
	/// @param _collection pipelineのコレクションクラスへのポインタ
	void PushBackRenderingData(RenderingPipelineCollection* _collection) override;

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

};

