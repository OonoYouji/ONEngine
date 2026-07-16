#pragma once

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Graphics/Pipelines/Render/Primitive/Line2DRenderingPipeline.h"
#include "Engine/Core/Utility/Math/Vector2.h"


/// ///////////////////////////////////////////////////
/// 2Dライン描画コンポーネント
/// ///////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class Line2DRenderer
 * @brief 2Dの線（ライン）を描画するためのレンダラーコンポーネントクラス
 */
class Line2DRenderer final : public IRenderComponent {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================
	
	/**
	 * @brief コンストラクタ
	 */
	Line2DRenderer();

	/**
	 * @brief デストラクタ
	 */
	~Line2DRenderer();

	/**
	 * @brief 描画する線分のパラメータを設定します。
	 * @param _start 始点のスクリーン座標（2D）
	 * @param _end 終点のスクリーン座標（2D）
	 * @param _color カラー（RGBA）
	 */
	void SetLine(const Vector2& _start, const Vector2& _end, const Vector4& _color);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	size_t lineCount_;
	Line2DRenderingPipeline::RenderingData renderingData_;


public:
	/// ===================================================
	/// public : accessor
	/// ===================================================
	
	/**
	 * @brief 描画パイプライン用データ（読み取り専用）を取得します。
	 */
	const Line2DRenderingPipeline::RenderingData& GetRenderingData() const { return renderingData_; }

	/**
	 * @brief 描画パイプライン用データのポインタを取得します。
	 */
	Line2DRenderingPipeline::RenderingData* GetRenderingDataPtr() { return &renderingData_; }

};


} /// ONEngine
