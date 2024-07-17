#include <Logger.h>

#include <Windows.h>

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


void ONE::Logger::ConsolePrint(const std::string& outputString) {
	OutputDebugStringA(("ONE : " + outputString).c_str());
}

void ONE::Logger::ConsolePrint(const std::wstring& outputString) {
	OutputDebugStringA(("ONE : " + ConvertString(outputString)).c_str());
}
