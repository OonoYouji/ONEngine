#pragma once

#include "../BaseCommand.h"

/// ===================================================
/// 新しいゲームオブジェクトを作成する用のクラス
/// ===================================================
class CreateNewGameObjectCMD final : public BaseCommand {
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

	/// ゲームオブジェクトの作成先
	const std::string directoryPath_ = "./Game/Objects/";

};