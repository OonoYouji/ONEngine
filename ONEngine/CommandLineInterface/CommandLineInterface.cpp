#include "CommandLineInterface.h"

#include <iostream>
#include <thread>

#include <Input.h>
#include <ImGuiManager.h>

#include "Commands/CreateNewCommandCMD/CreateNewCommandCMD.h"
#include "Commands/CreateNewGameObjectCMD/CreateNewGameObjectCMD.h"


BOOL WINAPI ConsoleHandler(DWORD signal) {
	switch(signal) {
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
		// コンソールウィンドウが閉じられたときの処理
		std::cout << "Console window closed. Game will continue running." << std::endl;
		// コンソールウィンドウを自由に閉じる
		FreeConsole();
		return TRUE;
	default:
		return FALSE;
	}
}


void CommandLineInterface::Initialize() {
	commandLog_.push_back("command line interface");

	/// コマンドの追加
	AddCommand("CreateGameObject", new CreateNewGameObjectCMD());
	AddCommand("CreateCommand",    new CreateNewCommandCMD());


	/// コンソールの初期化
	InitializeConsole();

}

void CommandLineInterface::Finalize() {
	FinalizeConsole();
}

void CommandLineInterface::RenderCLI() {
	for(auto log : commandLog_) {
		ImGui::TextUnformatted(log.c_str());
	}

}

void CommandLineInterface::ExecuteCommand(const std::string& commandInput) {
	commandLog_.push_back(commandInput);
}

void CommandLineInterface::CommandLoop() {

	

	std::string commandName;
	std::string commandArgs;
	while(isRenderConsole_) {

		/// 開く条件
		if(!GetConsoleWindow()) {
			if(Input::PressKey(KeyCode::LeftControl) && Input::TriggerKey(KeyCode::Space)) {
				SetConsoleCtrlHandler(ConsoleHandler, TRUE);
				AllocConsole();
				freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
				freopen_s((FILE**)stdin, "CONIN$", "r", stdin);

				std::cout << "Command Line Interface  Execution..." << std::endl;
				std::cout << "If you have any questions, please type 'help'." << std::endl;
			}
		} else {

			/// commandの名前を入力
			std::cout << "Name > ";
			std::getline(std::cin, commandName);


			auto itr = commands_.find(commandName);
			if(itr != commands_.end()) {

				/// 引数の入力
				std::cout << "    Args >> ";
				std::getline(std::cin, commandArgs);

				if(!commandArgs.empty()) {
					/// 実行
					itr->second->Execution(commandArgs);
					/// logに追加
					commandLog_.push_back(std::string(">> ") + commandName + std::string(" : ") + commandArgs);
				} else {
					std::cout << "         >> Args empty..." << std::endl;
				}

			} else {
				std::cout << "    >> This command is not available -> " << commandName << std::endl;
			}


			if(commandName == "Exit") {
				std::cout << "Exiting console..." << std::endl;
				FreeConsole();
				break;
 			}

		}

	}


	if(GetConsoleWindow()) {
		FreeConsole();
	}
}


void CommandLineInterface::InitializeConsole() {
	if(isRenderConsole_) {
		return;
	}

	isRenderConsole_ = true;

	commandLoop_ = std::thread(&CommandLineInterface::CommandLoop, this);
}


void CommandLineInterface::FinalizeConsole() {
	commandLoop_.join();
	isRenderConsole_ = false;
}


void CommandLineInterface::SetConsoleFont(const wchar_t* fontName, int fontSize) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);

	// 現在のフォント情報を取得
	if(GetCurrentConsoleFontEx(hConsole, FALSE, &cfi)) {
		// フォント名を設定
		wcscpy_s(cfi.FaceName, fontName);
		cfi.dwFontSize.Y = fontSize;  // 高さを設定
		cfi.FontWeight = FW_NORMAL;

		// 新しいフォントを設定
		SetCurrentConsoleFontEx(hConsole, FALSE, &cfi);
	} else {
		std::cerr << "Failed to get console font info." << std::endl;
		commandLog_.push_back("Failed to get console font info.");
	}
}

//void CommandLineInterface::AddCommand(const std::string& name, std::unique_ptr<BaseCommand> addCommand) {
//	auto itr = commands_.find(name);
//	if(itr != commands_.end()) {
//		std::cout << "Already created ->" << name << std::endl;
//		return;
//	}
//
//	commands_[name] = std::move(addCommand);
//
//}

void CommandLineInterface::AddCommand(const std::string& name, BaseCommand* addCommand) {
	auto itr = commands_.find(name);
	if(itr != commands_.end()) {
		std::cout << "Already created ->" << name << std::endl;
		return;
	}

	std::unique_ptr<BaseCommand> command(addCommand);
	commands_[name] = std::move(command);
}
