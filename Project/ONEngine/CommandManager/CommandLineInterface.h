#pragma once

#include <string>
#include <thread>
#include <list>
#include <memory>
#include <unordered_map>

#include "Commands/BaseCommand.h"

/// ===================================================
/// コマンドラインインターフェイスのクラス
/// ===================================================
class CommandLineInterface final {
	CommandLineInterface() {}
	~CommandLineInterface() {}
public:
	/// ===================================================
	/// public : static methods
	/// ===================================================

	static CommandLineInterface* GetInstance() {
		static CommandLineInterface instance;
		return &instance;
	}


	/// ===================================================
	/// public : methods
	/// ===================================================

	void Initialize();
	void Finalize();
	
	void RenderCLI();

	void ExecuteCommand(const std::string& commandInput);


	void CommandLoop();
	void InitializeConsole();
	void FinalizeConsole();

	void SetConsoleFont(const wchar_t* fontName, int fontSize);

	void AddCommand(const std::string& name, BaseCommand* addCommand);

	bool GetIsRenderConsole() const {
		return isRenderConsole_;
	}

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::list<std::string> commandLog_{};
	std::thread            commandLoop_;
	bool                   isRenderConsole_;

	/// commands
	std::unordered_map<std::string, std::unique_ptr<BaseCommand>> commands_;

private:
	CommandLineInterface(const CommandLineInterface&)             = delete;
	CommandLineInterface(CommandLineInterface&&)                  = delete;
	CommandLineInterface& operator= (const CommandLineInterface&) = delete;
	CommandLineInterface& operator= (CommandLineInterface&&)      = delete;
};