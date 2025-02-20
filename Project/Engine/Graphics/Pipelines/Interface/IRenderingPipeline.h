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
/// 描画の interface クラス
/// ===================================================
class IRenderingPipeline {
public:

	/// ===================================================
	/// public : virtual methods
	/// ===================================================

	virtual ~IRenderingPipeline() = default;

	/// @brief このクラスの初期化関数
	/// @param _shaderCompiler シェーダーのコンパイラーへのポインタ
	virtual void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) = 0;

	/// @brief 描画処理を行う
	/// @param _dxCommand DxCommandへのポインタ
	/// @param _entityCollection EntityCollectionへのポインタ
	virtual void Draw(DxCommand* _dxCommand, class EntityCollection* _entityCollection) = 0;


protected:

	/// ===================================================
	/// protected : objects
	/// ===================================================

	std::unique_ptr<GraphicsPipeline> pipeline_;

};


