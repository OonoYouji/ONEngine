#pragma once

/// std
#include <format>

/// ////////////////////////////////////////////////
/// Console Log
/// ////////////////////////////////////////////////
class Console final {
public:

	~Console();

	static void Log(const std::string& _message);
	static void Log(const std::wstring& _message);


	/// @brief logをfileに出力する
	/// @param _directory
	static void OutputLogToFile(const std::string& _directory);

	static const std::string& GetCurrentLog();
	static const std::string& GetAllLogs();
};


/*##########################################################
	TODO : COMMENT
	この関数を移動する先の.hを考える
##########################################################*/

std::string ConvertString(const std::wstring& _wstr);
std::wstring ConvertString(const std::string& _str);
