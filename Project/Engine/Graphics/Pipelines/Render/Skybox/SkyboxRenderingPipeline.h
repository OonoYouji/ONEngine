#pragma once

/// engine
#include "../../Interface/IRenderingPipeline.h"

#include "Engine/Core/DirectX12/Resource/DxResource.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"


/// /////////////////////////////////////////////////
/// 天球のレンダリングパイプライン
/// /////////////////////////////////////////////////
class SkyboxRenderingPipeline : public IRenderingPipeline {
public:

	struct VSInput {
		Vector4 position;
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	SkyboxRenderingPipeline(class AssetCollection* _assetCollection);
	~SkyboxRenderingPipeline();

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxm) override;
	void Draw(class ECSGroup* _ecs, const std::vector<GameEntity*>& _entities, class CameraComponent* _camera, DxCommand* _dxCommand) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	class AssetCollection* pAssetCollection_;
	ConstantBuffer<size_t> texIndex_;
	ConstantBuffer<Matrix4x4> transformMatrix_;
	
	
	std::vector<VSInput>         vertices_;
	DxResource                   vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW     vbv_;

	std::vector<uint32_t>        indices_;
	DxResource                   indexBuffer_;
	D3D12_INDEX_BUFFER_VIEW      ibv_;

};

