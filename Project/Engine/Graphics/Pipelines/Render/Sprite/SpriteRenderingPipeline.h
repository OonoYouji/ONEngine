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
#include "Engine/ECS/Entity/Entities/Camera/Camera.h"

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


public:
	
	/// ===================================================
	/// public : methods
	/// ===================================================

	SpriteRenderingPipeline(class GraphicsResourceCollection* _resourceCollection);
	~SpriteRenderingPipeline();


	/// @brief 初期化処理
	/// @param _shaderCompiler ShaderCompilerへのポインタ
	/// @param _dxManager DxManagerへのポインタ
	void Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) override;

	void Draw(class EntityComponentSystem* _ecs, const std::vector<IEntity*>& _entities, Camera* _camera, DxCommand* _dxCommand) override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	const size_t                                 kMaxRenderingSpriteCount_ = 1024; ///< 最大描画スプライト数

	class GraphicsResourceCollection*            resourceCollection_       = nullptr;

	std::unique_ptr<StructuredBuffer<uint32_t>>  textureIDsBuffer_;
	std::unique_ptr<StructuredBuffer<Matrix4x4>> transformsBuffer_;
	std::list<class SpriteRenderer*>             renderers_;
	
	std::vector<VertexData>                      vertices_;
	DxResource                                   vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW                     vbv_;

	std::vector<uint32_t>                        indices_;
	DxResource                                   indexBuffer_;
	D3D12_INDEX_BUFFER_VIEW                      ibv_;
	
};

