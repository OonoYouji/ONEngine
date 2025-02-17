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


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	void SetMeshPath(const std::string& _path) { meshPath_ = _path; }

	const std::string& GetMeshPath() const { return meshPath_; }

	void SetTexturePath(const std::string& _path) { texturePath_ = _path; }

	const std::string& GetTexturePath() const { return texturePath_; }


};

