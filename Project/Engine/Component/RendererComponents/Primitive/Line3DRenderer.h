#pragma once

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Graphics/Pipelines/Primitive/Line3DRenderingPipeline.h"
#include "Engine/Core/Utility/Math/Vector3.h"
#include "Engine/Core/Utility/Math/Vector4.h"

/// ===================================================
/// 3Dライン描画クラス
/// ===================================================
class Line3DRenderer final : public IComponent {
public:

	Line3DRenderer();
	~Line3DRenderer();

	/// @brief lineの設定
	/// @param _start 初期地
	/// @param _end   終了地
	/// @param _color ラインの色
	void SetLine(const Vector3& _start, const Vector3& _end, const Vector4& _color);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	size_t refCount_;
	std::vector<Line3DRenderingPipeline::VertexData> vertices_;


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	const std::vector<Line3DRenderingPipeline::VertexData>& GetVertices() const { return vertices_; }

};

