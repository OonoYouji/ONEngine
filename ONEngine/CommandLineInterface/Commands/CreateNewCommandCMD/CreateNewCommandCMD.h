#pragma once

#include "CommandLineInterface/Commands/BaseCommand.h"

/// ===================================================
/// 新しいコマンドのクラスを作成する用のクラス
/// ===================================================
class CreateNewCommandCMD final : public BaseCommand {
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

	/// 作成先のファイルパス
	const std::string directoryPath_ = "./ONEngine/CommandLineInterface/Commands/";

};