#include "Log.h"


#include <comdef.h>
#include <Windows.h>

/// std
#include <fstream>
#include <filesystem>
#include <chrono>

/// external
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/rotating_file_sink.h>

/// engine
#include "Engine/Core/Config/EngineConfig.h"

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
	std::vector<std::string> gMessages_;

	/// メンバ変数としてstaticで宣言したくないのでここで定義
	std::vector<std::string> gLogBuffer_;
	std::mutex gMutex_;

} /// namespace


/// ////////////////////////////////////////////////
/// Console Log
/// ////////////////////////////////////////////////


Console::~Console() {
#ifdef DEBUG_MODE
	OutputLogToFile("../Generated/Log");
#else 
	//OutputLogToFile("./Log");
#endif // DEBUG_MODE
}

void Console::Initialize() {

	/// 念のため一度だけ初期化するように制限をかける
	static bool initialized = false;
	if (initialized) {
		return;
	}

	/// 非同期スレッドプール
	spdlog::init_thread_pool(8192, 1);

	/// ログ出力先
	auto sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
		"../Generated/Log/engine.log", 10 * 1024 * 1024, 3);

	auto logger = std::make_shared<spdlog::async_logger>(
		"engine", sink,
		spdlog::thread_pool(),
		spdlog::async_overflow_policy::discard_new // v1.16.0の場合
	);

	spdlog::set_default_logger(logger);
	spdlog::set_pattern("[%H:%M:%S.%e] [%l] %v");

	spdlog::info("Logger initialized.");

	initialized = true;
}

void Console::AddToBuffer(const std::string& _msg) {
	std::lock_guard<std::mutex> lock(gMutex_);
	gLogBuffer_.push_back(_msg);

	/// ログの最大数を制限
	if (gLogBuffer_.size() > MAX_LOG_BUFFER_SIZE) {
		gLogBuffer_.erase(gLogBuffer_.begin());
	}
}


void Console::Log(const std::string& _message) {
	AddToBuffer(_message);
	spdlog::info(_message);
	//std::string formattedMessage;
	//std::istringstream stream(_message);
	//std::string line;
	//while (std::getline(stream, line)) {
	//	formattedMessage += GetCurrentTimeString() + line + "\n";
	//}
	//gMessage = formattedMessage;
	//gMessages_.push_back(gMessage);

	//gLogData.message += gMessage;
	////OutputDebugStringA(_message.c_str());
}

void Console::Log(const std::wstring& _message) {
	Log(ConvertString(_message));
}

void Console::LogInfo(const std::string& _message) {
	AddToBuffer("[info] " + _message);
	spdlog::info(_message);
}

void Console::LogError(const std::string& _message) {
	AddToBuffer("[error] " + _message);
	spdlog::error(_message);
}

void Console::LogWarning(const std::string& _message) {
	AddToBuffer("[warning] " + _message);
	spdlog::warn(_message);
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
	std::lock_guard<std::mutex> lock(gMutex_);
	std::string all;
	for (auto& m : gLogBuffer_) {
		all += m + "\n";
	}
	return all;
}

const std::vector<std::string>& Console::GetLogVector() {
	std::lock_guard<std::mutex> lock(gMutex_);
	return gLogBuffer_;
}


/// ////////////////////////////////////////////////
/// 文字列変換関数
/// ////////////////////////////////////////////////

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

std::string HrToString(HRESULT _hr) {
	_com_error err(_hr);
	const wchar_t* wmsg = err.ErrorMessage();

	// UTF-16 → UTF-8 変換
	int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wmsg, -1, nullptr, 0, nullptr, nullptr);
	std::string msg(sizeNeeded - 1, 0); // 終端を除く
	WideCharToMultiByte(CP_UTF8, 0, wmsg, -1, msg.data(), sizeNeeded, nullptr, nullptr);

	return msg;
}



