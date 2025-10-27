#pragma once

/// directX
#include <d3d11.h>

/// std
#include <vector>
#include <format>
#include <tchar.h>


/// @brief 最大のログバッファサイズ
static const size_t MAX_LOG_BUFFER_SIZE = 1000;


/// ////////////////////////////////////////////////
/// Console Log
/// ////////////////////////////////////////////////
class Console final {
	/// ----- friend class ----- ///
	friend class GameFramework;

private:
	/// ===================================================
	// private : static members
	/// ===================================================

	/// @brief spdlogの初期化など
	static void Initialize();

	static void AddToBuffer(const std::string& _msg);


public:

	~Console();

	static void Log(const std::string& _message);
	static void Log(const std::wstring& _message);
	static void LogInfo(const std::string& _message);
	static void LogError(const std::string& _message);
	static void LogWarning(const std::string& _message);


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

/// @brief DWORDを文字列に変換する
/// @param _dw 
/// @return 
std::string ConvertString(DWORD _dw);

/// @brief HRESULTを文字列に変換する
std::string HrToString(HRESULT _hr);