#pragma once

#include "../IEditCommand.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/Core/Utility/Math/Math.h"

namespace Editor {

/**
 * @class FocusEntityCommand
 * @brief エディタのヒエラルキービュー等でエンティティをダブルクリックした際、デバッグカメラの位置・向きを対象オブジェクトの正面に自動移動（フォーカス）させ、Undo時に移動前のカメラ位置に戻すコマンドクラス
 */
class FocusEntityCommand : public IEditCommand {
public:
	/// ==========================================
	/// public : methods
	/// ==========================================

	/**
	 * @brief コンストラクタ
	 * @param ecs ECSのポインタ（デバッグカメラの取得に必要）
	 * @param targetEntity フォーカス対象のエンティティ
	 */
	FocusEntityCommand(ONEngine::EntityComponentSystem* ecs, ONEngine::GameEntity* targetEntity);

	/**
	 * @brief デストラクタ
	 */
	~FocusEntityCommand() override = default;

	/**
	 * @brief コマンドの実行（デバッグカメラの位置を、対象エンティティが見える位置へ補間移動または瞬間移動させます）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Execute() override;

	/**
	 * @brief コマンドの取り消し処理（フォーカス実行前のカメラ座標・回転角度に戻します）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Undo() override;

private:
	/// ==========================================
	/// private : objects
	/// ==========================================

	ONEngine::EntityComponentSystem* pEcs_;
	ONEngine::GameEntity* targetEntity_;

	// Undo（取り消し）用に、フォーカス前のカメラの情報を保存しておく変数
	ONEngine::Vector3 previousPosition_;
	ONEngine::Quaternion previousRotation_;
	bool isExecuted_ = false;
};

} /// namespace Editor