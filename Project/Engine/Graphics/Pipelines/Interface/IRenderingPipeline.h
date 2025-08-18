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
class IRenderingPipeline {
public:

	/// ===================================================
	/// public : virtual methods
	/// ===================================================

	virtual ~IRenderingPipeline() = default;

	/// @brief このクラスの初期化関数
	/// @param _shaderCompiler シェーダーのコンパイラーへのポインタ
	virtual void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) = 0;

	virtual void PreDraw(class ECSGroup* _ecs, class CameraComponent* _camera, DxCommand* _dxCommand);

	/// @brief 描画処理を行う
	/// @param _dxCommand DxCommandへのポインタ
	/// @param _entityCollection EntityCollectionへのポインタ
	virtual void Draw(class ECSGroup* _ecs, const std::vector<class GameEntity*>& _entities, class CameraComponent* _camera, DxCommand* _dxCommand) = 0;

protected:

	/// ===================================================
	/// protected : objects
	/// ===================================================

	std::unique_ptr<GraphicsPipeline> pipeline_;

};


