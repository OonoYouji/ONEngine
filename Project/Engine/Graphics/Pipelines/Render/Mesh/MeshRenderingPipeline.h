#pragma once

/// std
#include <list>
#include <vector>
#include <memory>
#include <unordered_map>

/// engine
#include "../../Interface/IRenderingPipeline.h"
#include "Engine/Asset/Assets/Texture/Texture.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"
#include "Engine/Core/DirectX12/Resource/DxResource.h"
#include "Engine/Graphics/Buffer/Data/GPUMaterial.h"
#include "Engine/Core/Utility/Math/Matrix4x4.h"
#include "Engine/Core/Utility/Math/Vector4.h"


/// /////////////////////////////////////////////////
/// mesh描画クラス
/// /////////////////////////////////////////////////
class MeshRenderingPipeline final : public IRenderingPipeline {
public:

	/// ===================================================
	/// public : sub class
	/// ===================================================

	/// @brief 描画に必要なデータ
	struct RenderingData final {
		size_t              renderMeshId; /// TODO: stringに変更
		class MeshRenderer* meshRenderer;
	};

public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	MeshRenderingPipeline(class AssetCollection* _assetCollection);
	~MeshRenderingPipeline();

	/// @brief 初期化関数
	/// @param _shaderCompiler 
	/// @param _dxDevice 
	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxm) override;

	/// @brief 描画処理
	/// @param _dxCommand DxCommandへのポインタ
	/// @param _entityCollection EntityCollectionへのポインタ
	void Draw(class ECSGroup* _ecs, class CameraComponent* _camera, DxCommand* _dxCommand) override;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	void RenderingMesh(ID3D12GraphicsCommandList* _cmdList, std::unordered_map<std::string, std::list<class MeshRenderer*>>* _pMeshRendererPerMesh, const std::vector<Texture>& _pTexture);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	/// ----- other class ----- ///
	class AssetCollection* pAssetCollection_;


	const size_t kMaxRenderingMeshCount_ = 1024; ///< 最大描画メッシュ数

	StructuredBuffer<Matrix4x4> transformBuffer_;
	StructuredBuffer<GPUMaterial> materialBuffer_;
	StructuredBuffer<uint32_t> textureIdBuffer_;

	size_t transformIndex_;
	uint32_t instanceIndex_;

};

