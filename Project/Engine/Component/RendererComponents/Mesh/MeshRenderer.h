#pragma once

/// engine
#include "../../IComponent.h"
#include "Engine/Graphics/Pipelines/Mesh/MeshRenderingPipeline.h"

/// ===================================================
/// mesh描画クラス
/// ===================================================
class MeshRenderer : public IRenderComponent {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	MeshRenderer();
	~MeshRenderer();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::string meshPath_;
	std::string texturePath_;

	Vector4 color_ = Vector4::kWhite;

public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	void SetMeshPath(const std::string& _path) { meshPath_ = _path; }

	void SetTexturePath(const std::string& _path) { texturePath_ = _path; }

	void SetColor(const Vector4& _color) { color_ = _color; }


	const std::string& GetMeshPath() const { return meshPath_; }

	const std::string& GetTexturePath() const { return texturePath_; }

	const Vector4& GetColor() const { return color_; }

};

