#pragma once

/// directX
#include <d3d11.h>

/// std
#include <vector>
#include <format>
#include <tchar.h>

/// ////////////////////////////////////////////////
/// Console Log
/// ////////////////////////////////////////////////
class Console final {
public:

	~Console();

	static void Log(const std::string& _message);
	static void Log(const std::wstring& _message);
	static void LogInfo(const std::string& _message);
	static void LogError(const std::string& _message);
	static void LogWarning(const std::string& _message);


	/// @brief logをfileに出力する
	/// @param _directory
	static void OutputLogToFile(const std::string& _directory);

	/// @brief 最新のログを返す
	static const std::string& GetCurrentLog();

	/// @brief すべてのログを返す
	static const std::string& GetAllLogs();

	/// @brief ログのvectorを返す
	static const std::vector<std::string>& GetLogVector();
};


/// @brief wstring -> string 変換関数
std::string ConvertString(const std::wstring& _wstr);

/// @brief string -> wstring 変換関数
std::wstring ConvertString(const std::string& _str);

/// @brief TCHAR* -> string 変換関数
std::string ConvertTCHARToString(const TCHAR* _tstr);

/// @brief HRESULTを文字列に変換する
std::string HrToString(HRESULT _hr);