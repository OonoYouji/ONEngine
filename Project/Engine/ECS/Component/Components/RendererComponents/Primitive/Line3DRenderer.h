#pragma once

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Graphics/Pipelines/Render/Primitive/Line3DRenderingPipeline.h"
#include "Engine/Core/Utility/Math/Vector3.h"
#include "Engine/Core/Utility/Math/Vector4.h"

/// ///////////////////////////////////////////////////
/// 3Dライン描画クラス
/// ///////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class Line3DRenderer
 * @brief 3D空間上にデバッグ用などの線分（ライン）を描画するためのレンダラーコンポーネントクラス
 */
class Line3DRenderer final : public IComponent {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	Line3DRenderer();

	/**
	 * @brief デストラクタ
	 */
	~Line3DRenderer();

	/**
	 * @brief 描画する3D線分のパラメータを設定します。
	 * @param _start 始点の3Dワールド座標
	 * @param _end 終点の3Dワールド座標
	 * @param _color カラー（RGBA）
	 */
	void SetLine(const Vector3& _start, const Vector3& _end, const Vector4& _color);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	size_t lineCount_;
	std::vector<Line3DRenderingPipeline::VertexData> vertices_;


public:
	/// ===================================================
	/// public : accessor
	/// ===================================================

	/**
	 * @brief 描画する頂点データの配列（読み取り専用）を取得します。
	 */
	const std::vector<Line3DRenderingPipeline::VertexData>& GetVertices() const { return vertices_; }

};


} /// ONEngine
