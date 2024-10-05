#pragma once

#include "CommandLineInterface/Commands/BaseCommand.h"

/// ===================================================
/// 新しいコマンドのクラスを作成する用のクラス
/// ===================================================
class CreateNewCommandCMD final : BaseCommand {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	CreateNewCommandCMD() {}
	~CreateNewCommandCMD() {}

	void Execution(const std::string& args) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

};