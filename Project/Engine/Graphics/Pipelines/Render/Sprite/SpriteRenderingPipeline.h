#pragma once

/// std
#include <memory>
#include <vector>
#include <list>

/// engine
#include "../../Interface/IRenderingPipeline.h"
#include "Engine/Core/DirectX12/Resource/DxResource.h"
#include "Engine/Core/Utility/Math/Vector4.h"
#include "Engine/Core/Utility/Math/Vector2.h"
#include "Engine/Core/Utility/Math/Matrix4x4.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"
#include "Engine/Graphics/Buffer/Data/GPUMaterial.h"

/// /////////////////////////////////////////////////
/// sprite描画のパイプライン
/// /////////////////////////////////////////////////
class SpriteRenderingPipeline final : public IRenderingPipeline {
public:
	/// ===================================================
	/// public : sub class
	/// ===================================================

	/// @brief 頂点データ
	struct VertexData {
		Vector4 position;
		Vector2 uv;
	};

	enum ROOT_PARAM {
		ROOT_PARAM_VIEW_PROJECTION,
		ROOT_PARAM_MATERIAL,
		ROOT_PARAM_TEXTURES,
		ROOT_PARAM_TRANSFORM,
	};


public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	SpriteRenderingPipeline(class AssetCollection* _assetCollection);
	~SpriteRenderingPipeline();


	/// @brief 初期化処理
	/// @param _shaderCompiler ShaderCompilerへのポインタ
	/// @param _dxm DxManagerへのポインタ
	void Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) override;

	void Draw(class ECSGroup* _ecs, CameraComponent* _camera, DxCommand* _dxCommand) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// ----- other class ----- ///

	class AssetCollection* pAssetCollection_ = nullptr;


	const size_t                      kMaxRenderingSpriteCount_ = 1024; ///< 最大描画スプライト数

	StructuredBuffer<GPUMaterial>     materialsBuffer;
	StructuredBuffer<Matrix4x4>       transformsBuffer_;

	std::vector<VertexData>           vertices_;
	DxResource                        vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW          vbv_;

	std::vector<uint32_t>             indices_;
	DxResource                        indexBuffer_;
	D3D12_INDEX_BUFFER_VIEW           ibv_;

};

