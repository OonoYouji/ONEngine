#pragma once

/// window
#include <windows.h> 

/// std
#include <string>
#include <source_location>

/// engine
#include "LoggingManager/Logger.h"


inline void Assert(bool condition, const char* errorMessage, const std::source_location& location = std::source_location::current()) {
	if(!condition) {
		// エラーメッセージを作成
		std::string errorMsg = "Assertion failed: ";
		errorMsg += errorMessage;
		errorMsg += "\nFile: ";
		errorMsg += location.file_name();
		errorMsg += "\nFunction: ";
		errorMsg += location.function_name();
		errorMsg += "\nLine: ";
		errorMsg += std::to_string(location.line());

		ONE::Logger::ConsolePrint(errorMsg);

		// ポップアップウィンドウを表示
		MessageBoxA(nullptr, errorMsg.c_str(), "Assertion Failed", MB_OK | MB_ICONERROR);

		__debugbreak();
	}
}