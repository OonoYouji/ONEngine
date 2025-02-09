#pragma once

/// engine
#include "../Interface/IRenderer.h"

class MeshRenderer final : public IRenderer {
public:

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
	
	void DrawCall() override;

private:
};

