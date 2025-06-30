#include "Log.h"


#include <Windows.h>

/// std
#include <fstream>
#include <filesystem>
#include <chrono>

namespace {

	/// @brief 現在の年月日時間をstringで取得する
	/// @return 
	std::string GetCurrentDateTimeString() {
		std::time_t now = std::time(nullptr);
		std::tm timeInfo{};
		localtime_s(&timeInfo, &now);

		std::ostringstream oss;
		oss << std::setfill('0')
			<< (timeInfo.tm_year + 1900)
			<< std::setw(2) << (timeInfo.tm_mon + 1)
			<< std::setw(2) << timeInfo.tm_mday << "_"
			<< std::setw(2) << timeInfo.tm_hour
			<< std::setw(2) << timeInfo.tm_min
			<< std::setw(2) << timeInfo.tm_sec;

		return oss.str();
	}

	/// @brief 現在の時間をstringで取得する
	/// @return 
	std::string GetCurrentTimeString() {
		std::time_t now = std::time(nullptr);
		std::tm timeInfo{};
		localtime_s(&timeInfo, &now);
		std::ostringstream oss;
		oss << std::setw(2) << "["
			<< std::setw(2) << timeInfo.tm_hour << ":"
			<< std::setw(2) << timeInfo.tm_min << ":"
			<< std::setw(2) << timeInfo.tm_sec
			<< std::setw(2) << "] ";
		return oss.str();
	}

	/// ////////////////////////////////////////////
	/// LogData
	/// ////////////////////////////////////////////
	struct LogData {
		std::string message;
	};

	LogData gLogData;
	std::string gMessage;

} /// namespace


std::string ConvertString(const std::wstring& _wstr) {

	/// 引数が空の場合は空文字を返す
	if (_wstr.empty()) {
		return std::string();
	}

	/// 変換後のサイズを取得
	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, _wstr.data(), static_cast<int>(_wstr.size()), NULL, 0, NULL, NULL);
	if (sizeNeeded == 0) {
		return std::string();
	}

	/// 変換
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, _wstr.data(), static_cast<int>(_wstr.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;
}

std::wstring ConvertString(const std::string& _str) {

	/// 引数が空の場合は空文字を返す
	if (_str.empty()) {
		return std::wstring();
	}

	/// 変換後のサイズを取得
	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, _str.data(), static_cast<int>(_str.size()), NULL, 0);
	if (sizeNeeded == 0) {
		return std::wstring();
	}

	/// 変換
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, _str.data(), static_cast<int>(_str.size()), result.data(), sizeNeeded);
	return result;
}



std::string ConvertTCHARToString(const TCHAR* tstr) {
#ifdef UNICODE
	// TCHAR == wchar_t
	int len = WideCharToMultiByte(CP_UTF8, 0, tstr, -1, nullptr, 0, nullptr, nullptr);
	if (len == 0) return "";

	std::string result(len - 1, 0); // -1 to remove null terminator
	WideCharToMultiByte(CP_UTF8, 0, tstr, -1, result.data(), len, nullptr, nullptr);
	return result;
#else
	return std::string(tstr); // もともと char* ならそのまま
#endif
}




Console::~Console() {
	OutputLogToFile("../Generated/Log");
}

void Console::Log(const std::string& _message) {
	std::string formattedMessage;
	std::istringstream stream(_message);
	std::string line;
	while (std::getline(stream, line)) {
		formattedMessage += GetCurrentTimeString() + line + "\n";
	}
	gMessage = formattedMessage;

	gLogData.message += gMessage;
	OutputDebugStringA(gMessage.c_str());
}

void Console::Log(const std::wstring& _message) {
	std::string formattedMessage;
	std::istringstream stream(ConvertString(_message));
	std::string line;
	while (std::getline(stream, line)) {
		formattedMessage += GetCurrentTimeString() + line + "\n";
	}
	gMessage = formattedMessage;

	//gMessage = GetCurrentTimeStrinConvertString(_message)g() + ConvertString(_message) + "\n";
	gLogData.message += gMessage;
	OutputDebugStringA(gMessage.c_str());
}

void Console::OutputLogToFile(const std::string& _directory) {

	std::string fileName = "/log" + GetCurrentDateTimeString() + ".md";

	/// ファイルを開く
	std::ofstream file(_directory + fileName, std::ios::trunc);
	if (!file.is_open()) {
		std::filesystem::create_directories(_directory);
	}

	file << gLogData.message;
	file.close();
}

const std::string& Console::GetCurrentLog() {
	return gMessage;
}

const std::string& Console::GetAllLogs() {
	return gLogData.message;
}
