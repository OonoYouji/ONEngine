#include "CommandLineInterface.h"

#include <iostream>
#include <thread>

#include <ImGuiManager.h>


void CommandLineInterface::Initialize() {
	commandLog_.push_back("command line interface");

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

	if(!GetConsoleWindow()) {
		AllocConsole();
		SetConsoleFont(L"Consolas", 16); 
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
	}

	std::string command;
	while(isRenderConsole_) {
		std::cout << "> ";
		std::getline(std::cin, command);

		commandLog_.push_back(command);
		if(command == "exit") {
			std::cout << "Exiting...\n";
			isRenderConsole_ = false;
			break;
		} else if(!command.empty()) {
			std::cout << "Command entered: " << command << std::endl;
		}
	}

	FreeConsole();

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