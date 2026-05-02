#pragma once

/// window
#include <windows.h> 

/// std
#include <string>
#include <source_location>
#include <filesystem>

/// engine
#include "Console.h"


namespace Engine {

/// <summary>
/// _conditionがfalseの場合、エラーメッセージを表示してデバッグを停止する
/// </summary>
/// <param name="_condition">    : 止めるかどうかの条件、falseの場合止まる </param>
/// <param name="_errorMessage"> : メッセージボックスに表示するテキスト     </param>
inline void Assert(bool condition, const char* errorMessage, const std::source_location& location = std::source_location::current()) {
	if(!condition) {

		/// ファイルパスを取得
		std::filesystem::path filePath(location.file_name());

		/// エラーメッセージを作成
		std::string errorMsg = "Engine Assertion failed:\n";
		errorMsg += errorMessage;
		errorMsg += "\n\nLocation:\n";
		errorMsg += "File: ";
		errorMsg += filePath.filename().string();
		errorMsg += "\nFunction: ";
		errorMsg += location.function_name();
		errorMsg += "\nLine: ";
		errorMsg += std::to_string(location.line());

		/// ポップアップウィンドウを表示
		MessageBoxA(nullptr, errorMsg.c_str(), "ONEngine Assertion", MB_OK | MB_ICONERROR);
		Console::Log("[ASSERTION ERROR] " + errorMsg); // Log the last part if any

		Console::Shutdown();
		__debugbreak();
	}
}


/// <summary>
/// _conditionがfalseの場合、エラーメッセージを表示してデバッグを停止する
/// </summary>
/// <param name="_condition">    : 止めるかどうかの条件、falseの場合止まる </param>
inline void Assert(bool condition, const std::source_location& location = std::source_location::current()) {
	Assert(condition, "Assertion failed", location);
}

} /// namespace ONEngine