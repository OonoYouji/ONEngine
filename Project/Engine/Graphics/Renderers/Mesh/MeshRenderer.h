#pragma once

/// engine
#include "../Interface/IRenderer.h"
#include "Engine/DirectX12/Resource/DxResource.h"
#include "Engine/Utility/Math/Vector4.h"


/// ===================================================
/// mesh描画クラス
/// ===================================================
class MeshRenderer final : public IRenderer {
	
	/// ===================================================
	/// private : sub class
	/// ===================================================

	/// @brief 頂点データ
	struct VertexData {
		Vector4 position;
	};

public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	MeshRenderer();
	~MeshRenderer();

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

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	DxResource               vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vbv_;
	VertexData*              mappingData_ = nullptr;


};

