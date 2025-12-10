#pragma once

/// std
#include <memory>

/// engine
#include "Engine/Core/DirectX12/Command/DxCommand.h"
#include "Engine/Graphics/Shader/GraphicsPipeline.h"
#include "Engine/Graphics/Shader/ShaderCompiler.h"



/// ///////////////////////////////////////////////////
/// 描画の interface クラス
/// ///////////////////////////////////////////////////
namespace ONEngine {

class IRenderingPipeline {
public:
	/// ===================================================
	/// public : virtual methods
	/// ===================================================

	virtual ~IRenderingPipeline() = default;

	/// @brief このクラスの初期化関数
	/// @param _shaderCompiler シェーダーのコンパイラーへのポインタ
	virtual void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxm) = 0;

	/// @brief 描画前の処理を行う
	/// @param _ecs ECSGroupへのポインタ
	/// @param _camera 描画に用いるカメラへのポインタ
	/// @param _dxCommand DxCommandへのポインタ
	virtual void PreDraw(class ECSGroup* /*_ecs*/, class CameraComponent* /*_camera*/, DxCommand* /*_dxCommand*/) {}

	/// @brief 描画処理を行う
	/// @param _dxCommand DxCommandへのポインタ
	/// @param _entityCollection EntityCollectionへのポインタ
	virtual void Draw(class ECSGroup* _ecs, class CameraComponent* _camera, DxCommand* _dxCommand) = 0;

protected:
	/// ===================================================
	/// protected : objects
	/// ===================================================

	std::unique_ptr<GraphicsPipeline> pipeline_;
};



} /// ONEngine
