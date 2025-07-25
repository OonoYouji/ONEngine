#pragma once

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

	static const std::string& GetCurrentLog();
	static const std::string& GetAllLogs();
	static const std::vector<std::string>& GetLogVector();
};


/*##########################################################
	TODO : COMMENT
	この関数を移動する先の.hを考える
##########################################################*/

std::string ConvertString(const std::wstring& _wstr);
std::wstring ConvertString(const std::string& _str);
std::string ConvertTCHARToString(const TCHAR* _tstr);
