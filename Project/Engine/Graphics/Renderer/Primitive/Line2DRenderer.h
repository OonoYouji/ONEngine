#pragma once

#include "../Interface/IRenderer.h"


/// ===================================================
/// 2Dライン描画クラス
/// ===================================================
class Line2DRenderer : public IRenderer {
public:

	Line2DRenderer();
	~Line2DRenderer();

	/// @brief 初期化関数
	/// @param _shaderCompiler シェーダーコンパイラーへのポインタ
	void Initialize(ShaderCompiler* _shaderCompiler, DxDevice* _dxDevice) override;

	/// @brief 描画前の処理を行う
	/// @param _commandList DirectX12のコマンドリスト
	void PreDraw(ID3D12GraphicsCommandList* _commandList) override;

	/// @brief 描画後の処理を行う
	/// @param _commandList DirectX12のコマンドリスト
	void PostDraw(ID3D12GraphicsCommandList* _commandList) override;

	/// @brief 描画のコールを行う
	void DrawCall() override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

};

