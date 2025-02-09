#pragma once

/// std
#include <list>
#include <vector>

/// engine
#include "../Interface/IRenderingPipeline.h"
#include "Engine/Core/DirectX12/Resource/DxResource.h"
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
		size_t renderMeshId;
	};

public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	MeshRenderingPipeline();
	~MeshRenderingPipeline();

	/// @brief 初期化関数
	/// @param _shaderCompiler 
	/// @param _dxDevice 
	void Initialize(ShaderCompiler* _shaderCompiler, class DxDevice* _dxDevice) override;

	/// @brief 描画前の処理
	/// @param _dxCommand 
	void PreDraw(DxCommand* _dxCommand) override;

	/// @brief 描画後の処理
	/// @param _dxCommand 
	void PostDraw(DxCommand* _dxCommand) override;

	/// @brief 描画に必要なデータを追加
	/// @param _renderingData 
	void PushBackRenderingData(RenderingData* _renderingData) ;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::list<RenderingData*> renderingDataList_;

};

