#pragma once

/// window
#include <windows.h> 

/// std
#include <string>
#include <source_location>
#include <filesystem>
#include <csignal>

/// engine
#include "Log.h"
#include "Engine/Core/Config/EngineConfig.h"


namespace ONEngine {

/**
 * @brief 指定した条件が false の場合、エラーメッセージのダイアログを表示してプログラムの実行を一時停止（デバッグブレーク）します。
 * @param _condition アサーション条件。false の場合に停止します。
 * @param _errorMessage ダイアログやログに表示する詳細なエラーメッセージ
 * @param _location 呼び出し元のソースファイル位置情報（自動設定されます）
 */
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

		Console::Shutdown();
#if defined(_MSC_VER)
		__debugbreak();
#elif defined(__GNUC__) || defined(__clang__)
		__builtin_trap();
#else
		std::raise(SIGTRAP);
#endif
	}
}


/**
 * @brief 指定した条件が false の場合、デフォルトのメッセージでアサーションエラーを発生させます。
 * @param _condition アサーション条件。false の場合に停止します。
 * @param _location 呼び出し元のソースファイル位置情報（自動設定されます）
 */
inline void Assert(bool _condition, const std::source_location& _location = std::source_location::current()) {
	Assert(_condition, "Assertion failed", _location); ///< デフォルトのエラーメッセージを使用
}

} /// namespace ONEngine