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
#include "Engine/Graphics/Buffer/Data/Material.h"

/// ===================================================d
/// sprite描画のパイプライン
/// ===================================================d
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
		ROOT_PARAM_VIEW_PROJECTION = 0, ///< ビュープロジェクション行列
		ROOT_PARAM_MATERIAL,
		ROOT_PARAM_TEXTURES,
		ROOT_PARAM_TRANSFORM,
	};


public:
	
	/// ===================================================
	/// public : methods
	/// ===================================================

	SpriteRenderingPipeline(class AssetCollection* _resourceCollection);
	~SpriteRenderingPipeline();


	/// @brief 初期化処理
	/// @param _shaderCompiler ShaderCompilerへのポインタ
	/// @param _dxManager DxManagerへのポインタ
	void Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) override;

	void Draw(class ECSGroup* _ecs, const std::vector<GameEntity*>& _entities, CameraComponent* _camera, DxCommand* _dxCommand) override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	const size_t                                 kMaxRenderingSpriteCount_ = 1024; ///< 最大描画スプライト数

	class AssetCollection*            resourceCollection_       = nullptr;

	std::unique_ptr<StructuredBuffer<GPUMaterial>>  materialsBuffer;
	std::unique_ptr<StructuredBuffer<Matrix4x4>> transformsBuffer_;
	std::list<class SpriteRenderer*>             renderers_;
	
	std::vector<VertexData>                      vertices_;
	DxResource                                   vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW                     vbv_;

	std::vector<uint32_t>                        indices_;
	DxResource                                   indexBuffer_;
	D3D12_INDEX_BUFFER_VIEW                      ibv_;
	
};

