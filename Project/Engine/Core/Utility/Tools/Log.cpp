#include "Log.h"

#include <comdef.h>
#include <Windows.h>

/// std
#include <fstream>
#include <filesystem>
#include <chrono>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <ctime>

/// external
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/rotating_file_sink.h>

/// engine
#include "Engine/Core/Config/EngineConfig.h"

namespace ONEngine {

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


	std::string gMessage;

	/// メンバ変数としてstaticで宣言したくないのでここで定義
	std::vector<LogEntry> gLogBuffer_;
	std::mutex gMutex_;

} /// namespace


/// ////////////////////////////////////////////////
/// Console Log
/// ////////////////////////////////////////////////


void Console::Initialize() {

	/// 念のため一度だけ初期化するように制限をかける
	static bool initialized = false;
	if (initialized) {
		return;
	}

	/// 非同期スレッドプール
	spdlog::init_thread_pool(8192, 1);

	/// ログ出力先(日付入り)
#ifdef DEBUG_MODE
	const std::string logDir = "../Generated/Log/";
#else 
	const std::string logDir = "./Log/";
#endif
	const std::string fileName = "engine" + GetCurrentDateTimeString() + ".log";
	auto sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
		logDir + fileName, 10 * 1024 * 1024, 3);

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

void Console::Finalize() {
	spdlog::info("Logger finalized.");
	spdlog::shutdown();
}

void Console::AddToBuffer(const std::string& _msg, LogLevel _level, LogCategory _category) {
	std::lock_guard<std::mutex> lock(gMutex_);
	gLogBuffer_.push_back({ _level, _category, _msg });

	/// ログの最大数を制限
	if (gLogBuffer_.size() > MAX_LOG_BUFFER_SIZE) {
		gLogBuffer_.erase(gLogBuffer_.begin());
	}
}


Console::~Console() {}

void Console::Log(const std::string& _message, LogCategory _category) {
	AddToBuffer(_message, LogLevel::Info, _category);
	spdlog::info(_message);
	OutputDebugStringA(("[Log] " + _message + "\n").c_str());
}

void Console::Log(const std::wstring& _message, LogCategory _category) {
	Log(ConvertString(_message), _category);
}

void Console::LogInfo(const std::string& _message, LogCategory _category) {
	AddToBuffer(_message, LogLevel::Info, _category);
	spdlog::info(_message);
	OutputDebugStringA(("[Info] " + _message + "\n").c_str());
}

void Console::LogError(const std::string& _message, LogCategory _category) {
	AddToBuffer(_message, LogLevel::Error, _category);
	spdlog::error(_message);
	OutputDebugStringA(("[Error] " + _message + "\n").c_str());
}

void Console::LogWarning(const std::string& _message, LogCategory _category) {
	AddToBuffer(_message, LogLevel::Warning, _category);
	spdlog::warn(_message);
	OutputDebugStringA(("[Warning] " + _message + "\n").c_str());
}

const std::vector<LogEntry>& Console::GetLogVector() {
	return gLogBuffer_;
}

void Console::ClearLogBuffer(std::optional<LogCategory> _category) {
	std::lock_guard<std::mutex> lock(gMutex_);
	if (!_category.has_value()) {
		gLogBuffer_.clear();
	} else {
		gLogBuffer_.erase(
			std::remove_if(gLogBuffer_.begin(), gLogBuffer_.end(),
				[_category](const LogEntry& entry) {
					return entry.category == _category.value();
				}),
			gLogBuffer_.end()
		);
	}
}

void Console::Shutdown() {
	Finalize();
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

std::string ConvertString(DWORD _dw) {
	return std::to_string(_dw);
}

std::string HrToString(HRESULT _hr) {
	char* errorMsg = nullptr;

	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		_hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&errorMsg),
		0,
		nullptr
	);

	std::string errorString = errorMsg ? errorMsg : "Unknown error";
	LocalFree(errorMsg); // メモリを解放

	return errorString;
}

} /// namespace ONEngine
