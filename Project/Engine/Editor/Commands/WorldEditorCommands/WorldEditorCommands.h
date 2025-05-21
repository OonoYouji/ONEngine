#pragma once

/// engine
#include "../Interface/IEditorCommand.h"


/// ///////////////////////////////////////////////////
/// ゲームオブジェクトの作成コマンド
/// ///////////////////////////////////////////////////
class CreateGameObjectCommand : public IEditorCommand {
public:
	/// =========================================
	/// public : methods
	/// =========================================

	CreateGameObjectCommand();
	~CreateGameObjectCommand();

	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

};
