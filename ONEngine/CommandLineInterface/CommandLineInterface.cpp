#include "CommandLineInterface.h"

#include <iostream>
#include <thread>

#include <ImGuiManager.h>

void SetConsoleFont(const wchar_t* fontName, int fontSize) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	GetCurrentConsoleFontEx(hConsole, FALSE, &cfi);

	// フォント名とサイズを設定
	wcscpy_s(cfi.FaceName, fontName);
	cfi.dwFontSize.Y = fontSize;  // 高さを設定
	cfi.FontWeight = FW_NORMAL;

	SetCurrentConsoleFontEx(hConsole, FALSE, &cfi);
}

void CommandLineInterface::Initialize() {
	commandLog_.push_back("command line interface");

	//SetConsoleFont(L"Terminal", 16);
	//InitializeConsole();
	//
}

void CommandLineInterface::Finalize() {
	//FinalizeConsole();
}

void CommandLineInterface::RenderCLI() {
	/*for(auto log : commandLog_) {
		ImGui::TextUnformatted(log.c_str());
	}

	size_t bufferSize = sizeof(char) * 256;
	if(ImGui::InputText("##CommandInput", commandInput_.data(), bufferSize, ImGuiInputTextFlags_EnterReturnsTrue)) {
		if(!commandInput_.empty()) {
			ExecuteCommand(commandInput_);
			commandInput_.clear();
		}
	}*/
}

void CommandLineInterface::ExecuteCommand(const std::string& commandInput) {
	commandLog_.push_back(commandInput);
}

void CommandLineInterface::CommandLoop() {

	if(!GetConsoleWindow()) {
		AllocConsole();
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
		std::cout << "Console initialized.\n";
	}

	std::string command;
	while(isRenderConsole_) {
		std::cout << "> ";
		std::getline(std::cin, command);
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

	std::thread func(&CommandLineInterface::CommandLoop, this);
	commandLoop_.swap(func);
}


void CommandLineInterface::FinalizeConsole() {
	commandLoop_.join();
}
