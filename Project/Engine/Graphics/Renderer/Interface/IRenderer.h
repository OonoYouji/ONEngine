#pragma once

/// directX
#include <d3d12.h>

/// std
#include <memory>

/// engine
#include "Engine/Graphics/Shader/GraphicsPipeline.h"
#include "Engine/Graphics/Shader/ShaderCompiler.h"
//#include "Engine/DirectX12/Device/DxDevice.h"


/// ===================================================
/// 描画のinterfaceクラス
/// ===================================================
class IRenderer {
public:

	/// ===================================================
	/// public : virtual methods
	/// ===================================================

	virtual ~IRenderer() = default;

	/// @brief このクラスの初期化関数
	/// @param _shaderCompiler シェーダーのコンパイラーへのポインタ
	virtual void Initialize(ShaderCompiler* _shaderCompiler, class DxDevice* _dxDevice) = 0;

	/// @brief 描画前の処理を行う
	/// @param _commandList dxManagerが管理しているcommandListへのポインタ
	virtual void PreDraw(ID3D12GraphicsCommandList* _commandList)  = 0;

	/// @brief 描画後の処理を行う
	/// @param _commandList dxManagerが管理しているcommandListへのポインタ
	virtual void PostDraw(ID3D12GraphicsCommandList* _commandList) = 0;

	/// @brief 描画のコールを行う
	virtual void DrawCall() = 0;


protected:

	/// ===================================================
	/// protected : objects
	/// ===================================================

	std::unique_ptr<GraphicsPipeline> pipeline_;

};


