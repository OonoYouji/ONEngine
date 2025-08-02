#pragma once

/// window
#include <windows.h> 

/// std
#include <string>
#include <source_location>
#include <filesystem>

/// engine
#include "Log.h"
#include "Engine/Core/Config/EngineConfig.h"


/// <summary>
/// _conditionがfalseの場合、エラーメッセージを表示してデバッグを停止する
/// </summary>
/// <param name="_condition">    : 止めるかどうかの条件、falseの場合止まる </param>
/// <param name="_errorMessage"> : メッセージボックスに表示するテキスト     </param>
inline void Assert(bool _condition, const char* _errorMessage, const std::source_location& _location = std::source_location::current()) {
	if (!_condition) {

		/// ファイルパスを取得
		std::filesystem::path filePath(_location.file_name());

		/// エラーメッセージを作成
		std::string errorMsg = "ONEngine Assertion failed:\n";
		errorMsg += _errorMessage;
		errorMsg += "\n\nLocation:\n";
		errorMsg += "File: ";
		errorMsg += filePath.filename().string();
		errorMsg += "\nFunction: ";
		errorMsg += _location.function_name();
		errorMsg += "\nLine: ";
		errorMsg += std::to_string(_location.line());

		/// ポップアップウィンドウを表示
		MessageBoxA(nullptr, errorMsg.c_str(), "ONEngine Assertion", MB_OK | MB_ICONERROR);
		Console::Log("[ASSERTION ERROR] " + errorMsg); // Log the last part if any

#ifdef DEBUG_MODE
		Console::OutputLogToFile("../Generated/Log"); ///< 停止する前にログを出力する
#else
		Console::OutputLogToFile("./Log");
#endif // DEBUG_MODE

		__debugbreak();
	}
}


/// <summary>
/// _conditionがfalseの場合、エラーメッセージを表示してデバッグを停止する
/// </summary>
/// <param name="_condition">    : 止めるかどうかの条件、falseの場合止まる </param>
inline void Assert(bool _condition, const std::source_location& _location = std::source_location::current()) {
	Assert(_condition, "Assertion failed", _location); ///< デフォルトのエラーメッセージを使用
}