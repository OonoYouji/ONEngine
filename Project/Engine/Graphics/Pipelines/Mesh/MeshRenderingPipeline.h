#pragma once

/// std
#include <list>
#include <vector>
#include <memory>

/// engine
#include "../Interface/IRenderingPipeline.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Core/DirectX12/Resource/DxResource.h"
#include "Engine/Core/Utility/Math/Matrix4x4.h"
#include "Engine/Core/Utility/Math/Vector4.h"


/// ===================================================
/// mesh描画クラス
/// ===================================================
class MeshRenderingPipeline final : public IRenderingPipeline {
public:

	/// ===================================================
	/// public : sub class
	/// ===================================================

	/// @brief 頂点データ
	struct VertexData {
		Vector4 position;
	};

	/// @brief 描画に必要なデータ
	struct RenderingData final {
		size_t              renderMeshId; /// TODO: stringに変更
		class MeshRenderer* meshRenderer;
	};

public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	MeshRenderingPipeline(class GraphicsResourceCollection* _resourceCollection, class EntityCollection* _entityCollection);
	~MeshRenderingPipeline();

	/// @brief 初期化関数
	/// @param _shaderCompiler 
	/// @param _dxDevice 
	void Initialize(ShaderCompiler* _shaderCompiler, class DxDevice* _dxDevice) override;

	/// @brief 描画処理
	/// @param _dxCommand DxCommandへのポインタ
	/// @param _entityCollection EntityCollectionへのポインタ
	void Draw(DxCommand* _dxCommand, class EntityCollection* _entityCollection);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class GraphicsResourceCollection*          resourceCollection_;
	class EntityCollection*                    entityCollection_;
	std::list<RenderingData*>                  renderingDataList_;
	std::unique_ptr<ConstantBuffer<Matrix4x4>> transformBuffer_;


};

