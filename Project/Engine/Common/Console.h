#pragma once

/// directX
#include <d3d11.h>

/// std
#include <vector>
#include <format>
#include <tchar.h>


/// @brief 最大のログバッファサイズ
static const size_t MAX_LOG_BUFFER_SIZE = 2147483648;


namespace Engine {

///
/// ログ出力用のクラス
///
class Console final {
private:
	/// ===================================================
	// private : static members
	/// ===================================================

	static void AddToBuffer(const std::string& msg);

public:

	~Console();

	static void Log(const std::string& msg);
	static void Log(const std::wstring& msg);
	static void LogInfo(const std::string& msg);
	static void LogError(const std::string& msg);
	static void LogWarning(const std::string& msg);
	/// @brief ログのvectorを返す
	static const std::vector<std::string>& GetLogVector();

	/// @brief ログをファイルに保存して終了する
	static void Initialize();
	static void Shutdown();
};


/// @brief wstring -> string 変換関数
std::string ConvertString(const std::wstring& wstr);

/// @brief string -> wstring 変換関数
std::wstring ConvertString(const std::string& str);

/// @brief TCHAR* -> string 変換関数
std::string ConvertTCHARToString(const TCHAR* tstr);

/// @brief DWORDを文字列に変換する
/// @param dw 
/// @return 
std::string ConvertString(DWORD dw);

/// @brief HRESULTを文字列に変換する
std::string HrToString(HRESULT hr);

} /// ONEngine
