#pragma once

#include <format>

namespace ONE {

	/// ===================================================
	/// ログ出力
	/// ===================================================
	namespace Logger {

		void ConsolePrint(const std::string& outputString);
		void ConsolePrint(const std::wstring& outputString);


	}

	std::string ConvertString(const std::wstring& str);
}