#pragma once

#include <string>
#include <list>
#include <thread>

class CommandLineInterface final {
	CommandLineInterface() {}
	~CommandLineInterface() {}
public:

	static CommandLineInterface* GetInstance() {
		static CommandLineInterface instance;
		return &instance;
	}

	void Initialize();
	void Finalize();
	
	void RenderCLI();

	void ExecuteCommand(const std::string& commandInput);


	void CommandLoop();
	void InitializeConsole();
	void FinalizeConsole();

	void SetConsoleFont(const wchar_t* fontName, int fontSize);

private:

	std::string            commandInput_{};
	std::list<std::string> commandLog_{};
	std::thread            commandLoop_;
	bool                   isRenderConsole_;

private:
	CommandLineInterface(const CommandLineInterface&)             = delete;
	CommandLineInterface(CommandLineInterface&&)                  = delete;
	CommandLineInterface& operator= (const CommandLineInterface&) = delete;
	CommandLineInterface& operator= (CommandLineInterface&&)      = delete;
};