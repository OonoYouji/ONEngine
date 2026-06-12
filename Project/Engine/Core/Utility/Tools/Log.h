#pragma once

/// directX
#include <d3d11.h>

/// std
#include <vector>
#include <string>
#include <format>
#include <tchar.h>
#include <optional>


/// @brief 最大のログバッファサイズ
static const size_t MAX_LOG_BUFFER_SIZE = 2000;


/// ////////////////////////////////////////////////
/// Console Log
/// ////////////////////////////////////////////////
namespace ONEngine {

enum class LogLevel {
	Info,
	Warning,
	Error
};

enum class LogCategory {
	Engine,
	ScriptEngine,
	Application
};

struct LogEntry {
	LogLevel level;
	LogCategory category;
	std::string message;
};

class Console final {
	/// ----- friend class ----- ///
	friend class GameFramework;

private:
	/// ===================================================
	// private : static members
	/// ===================================================

	/// @brief spdlogの初期化など
	static void Initialize();

	/// @brief spdlogの終了処理など
	static void Finalize();

	static void AddToBuffer(const std::string& _msg, LogLevel _level, LogCategory _category);


public:

	~Console();

	static void Log(const std::string& _message, LogCategory _category = LogCategory::Engine);
	static void Log(const std::wstring& _message, LogCategory _category = LogCategory::Engine);
	static void LogInfo(const std::string& _message, LogCategory _category = LogCategory::Engine);
	static void LogError(const std::string& _message, LogCategory _category = LogCategory::Engine);
	static void LogWarning(const std::string& _message, LogCategory _category = LogCategory::Engine);

	/// @brief ログのvectorを返す
	static const std::vector<LogEntry>& GetLogVector();

	/// @brief ログのバッファをクリアする
	/// @param _category 指定したカテゴリのみクリアする。デフォルト(std::nullopt)は全てクリア
	static void ClearLogBuffer(std::optional<LogCategory> _category = std::nullopt);

	/// @brief ログをファイルに保存して終了する
	static void Shutdown();
};


/// @brief wstring -> string 変換関数
std::string ConvertString(const std::wstring& _wstr);

/// @brief string -> wstring 変換関数
std::wstring ConvertString(const std::string& _str);

/// @brief TCHAR* -> string 変換関数
std::string ConvertTCHARToString(const TCHAR* _tstr);

/// @brief DWORDを文字列に変換する
/// @param _dw 
/// @return 
std::string ConvertString(DWORD _dw);

/// @brief HRESULTを文字列に変換する
std::string HrToString(HRESULT _hr);

} /// ONEngine
