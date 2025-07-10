#pragma once

/// std
#include <numbers>
#include <string>
#include <vector>

/// @brief fileの相対パスと名前のペア
using File = std::pair<std::string, std::string>;


namespace Mathf {

	static const float Deg2Rad = std::numbers::pi_v<float> / 180.0f;
	static const float Rad2Deg = 180.0f / std::numbers::pi_v<float>;
	static const float PI = std::numbers::pi_v<float>;

	/// ----- ファイル ----- ///

	std::vector<File> FindFiles(const std::string& _fileDirectory, const std::string& _fileExtension);
	File FindFile(const std::string& _fileDirectory, const std::string& _filename);
	
	void ReplaceAll(std::string* _str, const std::string& _from, const std::string& _to);

	std::string FileNameWithoutExtension(const std::string& _filename);

	std::vector<std::vector<int>> LoadCSV(const std::string& _filePath);

}


