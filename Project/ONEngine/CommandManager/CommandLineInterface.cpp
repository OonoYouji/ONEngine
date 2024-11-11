#include "CommandLineInterface.h"

#include <iostream>
#include <thread>

#include <Core/ONEngine.h>
#include "Input/Input.h"
#include "ImGuiManager/ImGuiManager.h"
#include "WindowManager/WinApp.h"

#include "Commands/CreateNewCommandCMD/CreateNewCommandCMD.h"
#include "Commands/CreateNewGameObjectCMD/CreateNewGameObjectCMD.h"


// コントロールハンドラ
BOOL WINAPI ConsoleHandler(DWORD signal) {
	switch(signal) {
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
		// コンソールのクローズを無視する
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


	///// コンソールの初期化
	//InitializeConsole();

}

void CommandLineInterface::Finalize() {
	//FinalizeConsole();
}

void CommandLineInterface::RenderCLI() {
	for(auto log : commandLog_) {
		ImGui::TextUnformatted(log.c_str());
	}

}

void CommandLineInterface::ExecuteCommand(const std::string& commandInput) {
	commandLog_.push_back(commandInput);
}

void CommandLineInterface::ExecuteCommand(const std::string& _commandKey, const std::string& _commandArgs) {
	commands_[_commandKey]->Execution(_commandArgs);
}

void CommandLineInterface::CommandLoop() {

	
	HWND consoleHWND = nullptr;
	std::string commandName;
	std::string commandArgs;
	while(isRenderConsole_) {

		/// 開く条件
		if(!GetConsoleWindow()) {
			if(Input::PressKey(KeyCode::LeftControl) && Input::TriggerKey(KeyCode::Space)) {
				AllocConsole();
				SetConsoleCtrlHandler(ConsoleHandler, TRUE);
				freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
				freopen_s((FILE**)stdin, "CONIN$", "r", stdin);

				std::cout << "Command Line Interface  Execution..." << std::endl;
				std::cout << "If you have any questions, please type 'help'." << std::endl;

				consoleHWND = GetConsoleWindow();
				HMENU hMenu = GetSystemMenu(consoleHWND, false);

				if(hMenu) {
					EnableMenuItem(hMenu, SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
				}
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

		if(!ONEngine::GetIsRunning()) {
			break;
		}

	}


	if(GetConsoleWindow()) {
		FreeConsole();
	}
}


void CommandLineInterface::InitializeConsole() {

	std::system("start wsl.exe");

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
