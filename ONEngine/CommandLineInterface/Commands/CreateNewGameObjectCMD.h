#pragma once

#include "CommandLineInterface/Commands/BaseCommand.h"

/// ===================================================
/// 新しいゲームオブジェクトを作成する用のクラス
/// ===================================================
class CreateNewGameObjectCMD final : BaseCommand {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	CreateNewGameObjectCMD() {}
	~CreateNewGameObjectCMD() {}

	void Execution(const std::string& args) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

};