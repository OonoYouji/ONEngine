#include "Log.h"

#include <Windows.h>

void Log(const std::string& _message) {
	OutputDebugStringA(("### ONE : " + _message + "\n").c_str());
}

void Log(const std::wstring& _message) {
	OutputDebugStringA(("### ONE : " + ConvertString(_message) + "\n").c_str());
}


std::string ConvertString(const std::wstring& _wstr) {

	/// 引数が空の場合は空文字を返す
	if(_wstr.empty()) {
		return std::string();
	}

	/// 変換後のサイズを取得
	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, _wstr.data(), static_cast<int>(_wstr.size()), NULL, 0, NULL, NULL);
	if(sizeNeeded == 0) {
		return std::string();
	}

	/// 変換
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, _wstr.data(), static_cast<int>(_wstr.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;
}
