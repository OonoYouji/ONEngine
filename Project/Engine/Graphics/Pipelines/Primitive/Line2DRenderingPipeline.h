#pragma once

/// engine
#include "../Interface/IRenderingPipeline.h"
#include "Engine/Utility/Math/Vector4.h"
#include "Engine/DirectX12/Resource/DxResource.h"


/// ===================================================
/// 2Dライン描画クラス
/// ===================================================
class Line2DRenderingPipeline : public IRenderingPipeline {
public:

	/// ===================================================
	/// public : nest class
	/// ===================================================

	struct VertexData {
		Vec4 position;
		Vec4 color;
	};

public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Line2DRenderingPipeline();
	~Line2DRenderingPipeline();

	/// @brief 初期化関数
	/// @param _shaderCompiler シェーダーコンパイラーへのポインタ
	void Initialize(ShaderCompiler* _shaderCompiler, DxDevice* _dxDevice) override;

	/// @brief 描画前の処理を行う
	/// @param _commandList DirectX12のコマンドリスト
	void PreDraw(DxCommand* _dxCommand) override;

	/// @brief 描画後の処理を行う
	/// @param _commandList DirectX12のコマンドリスト
	void PostDraw(DxCommand* _dxCommand) override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	const size_t kMaxVertexNum_ = 1024; ///< 最大ライン数

	DxResource                  vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW    vbv_;
	VertexData*                 mappingData_  = nullptr;


	std::vector<VertexData> vertices_;

};

