#include <Logger.h>

#include <Windows.h>



/// ===================================================
/// 無記名名前空間 : cppで使う関数を定義
/// ===================================================
namespace {
	std::string ConvertString(const std::wstring& str) {
		if(str.empty()) {
			return std::string();
		}

		auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
		if(sizeNeeded == 0) {
			return std::string();
		}
		std::string result(sizeNeeded, 0);
		WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
		return result;
	}
} ///- namespace



/// ===================================================
/// string用コンソールログ出力
/// ===================================================
void ONE::Logger::ConsolePrint(const std::string& outputString) {
	OutputDebugStringA(("### ONE : " + outputString + "\n").c_str());
}


/// ===================================================
/// wstring用コンソールログ出力
/// ===================================================
void ONE::Logger::ConsolePrint(const std::wstring& outputString) {
	OutputDebugStringA(("### ONE : " + ConvertString(outputString) + "\n").c_str());
}
