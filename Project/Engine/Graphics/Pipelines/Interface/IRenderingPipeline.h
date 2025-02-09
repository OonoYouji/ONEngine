#pragma once

/*
* preDraw: pipelineのセット、前フレームのデータのリセット
*/



/// std
#include <memory>

/// engine
#include "Engine/Core/DirectX12/Command/DxCommand.h"
#include "Engine/Graphics/Shader/GraphicsPipeline.h"
#include "Engine/Graphics/Shader/ShaderCompiler.h"


/// ===================================================
/// 描画のinterfaceクラス
/// ===================================================
class IRenderingPipeline {
public:

	/// ===================================================
	/// public : virtual methods
	/// ===================================================

	virtual ~IRenderingPipeline() = default;

	/// @brief このクラスの初期化関数
	/// @param _shaderCompiler シェーダーのコンパイラーへのポインタ
	virtual void Initialize(ShaderCompiler* _shaderCompiler, class DxDevice* _dxDevice) = 0;

	/// @brief 描画前の処理を行う
	/// @param _commandList dxManagerが管理しているcommandListへのポインタ
	virtual void PreDraw(DxCommand* _dxCommand)  = 0;

	/// @brief 描画後の処理を行う
	/// @param _commandList dxManagerが管理しているcommandListへのポインタ
	virtual void PostDraw(DxCommand* _dxCommand) = 0;

protected:

	/// ===================================================
	/// protected : objects
	/// ===================================================

	std::unique_ptr<GraphicsPipeline> pipeline_;

};


