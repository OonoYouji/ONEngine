#pragma once

/// std
#include <memory>
#include <vector>

/// engine
#include "../../Interface/IRenderingPipeline.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"

#include "Engine/Asset/Assets/Mesh/Skinning.h"
#include "Engine/Graphics/Buffer/Data/GPUMaterial.h"


namespace ONEngine {
class DxManager;
class ECSGroup;
class CameraComponent;
class DxCommand;
}

namespace ONEngine::Asset {
class AssetCollection;
}



/// //////////////////////////////////////////////////////
/// スキンアニメーションの描画パイプライン
/// //////////////////////////////////////////////////////
namespace ONEngine {

class SkinMeshRenderingPipeline : public IRenderingPipeline {

	/// @brief インスタンスごとのデータ構造体
	struct SkinMeshInstanceData {
		Matrix4x4 matWorld;
		GPUMaterial material;
	};

	enum {
		ViewProjectionCBV = 0, ///< ViewProjectionのCBV (b0)
		InstanceIndexCBV,      ///< インスタンスインデックス (b1)
		InstanceDataSRV,       ///< 全インスタンスデータのSRV (t0)
		WellForGPUSRV,         ///< モデルごとのボーンパレットのSRV (t1)
		TextureSRV,            ///< テクスチャ配列のSRV (t2)
	};

public:
	/// ====================================================
	/// public : methods
	/// ====================================================

	SkinMeshRenderingPipeline(Asset::AssetCollection* _assetCollection);
	~SkinMeshRenderingPipeline() override = default;

	void Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) override;
	void Draw(ECSGroup* _ecs, CameraComponent* _camera, DxCommand* _dxCommand) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	Asset::AssetCollection* pAssetCollection_ = nullptr;

	static constexpr size_t kMaxInstances = 1024;
	
	/// インスタンスデータを一括管理するバッファ
	StructuredBuffer<SkinMeshInstanceData> instanceDataBuffer_;
	std::vector<SkinMeshInstanceData> instanceDataCPU_;

};


} /// ONEngine
